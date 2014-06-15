// Luke Monaghan
// ArduinoLEDs

#include "Math.h"

// Pinouts
// 6,5,3 are LEDs
// 8 is GND
byte Pins[4] = {6,5,3,8};

// Speed
short SPEED = 10;

// LED Colours
short col[3] = {0,0,0};

// Custom Command Buffer
char CommandLoop[128];

// Command ID
byte com = 0;

// Scrolling Control
float ScrollTime = 0;
byte selected = 0;

// Arduino Setup
void setup() {
	// random seed from analog, good enough
	randomSeed(analogRead(0));
	// our pins
	pinMode(Pins[0], OUTPUT);
	pinMode(Pins[1], OUTPUT);
	pinMode(Pins[2], OUTPUT);
	pinMode(Pins[3], OUTPUT);
	
	//Serial
	Serial.begin(9600);

	// boot Sequence
	SpecialCommand("r2r2g2g2b2b2w2o0");

	// Now scroll
	com = 2;
}

void SpecialCommand(char _Command[]){
	bool leds[3] = {0,0,0};

	for (byte b = 0; b <= strlen(_Command); b++){
		// check if we have a new command, if we do, exit out
		if ( Serial.available() > 0 ){return;}
		if ( (_Command[b] <= '9' && _Command[b] >= '0') || _Command[b] == ' ' || _Command[b] == '#' ) { 
			if (_Command[b] == '#'){ // Random
				delay(100.0f * random(0,11));
			}else if (_Command[b] == '0' || _Command[b] == ' '){ // Full delay
				delay(1000.0f);
			}else{ // Percent Delay
				char buffer[2] = "9";
				buffer[0] = _Command[b];
				int val = atoi(buffer);
				delay(1000.0f * (val / 10.0f));
			}
		}
		// RGB W and O
		if (_Command[b] == 'r' || _Command[b] == 'R'){leds[0] = !leds[0];}
		if (_Command[b] == 'g' || _Command[b] == 'G'){leds[1] = !leds[1];}
		if (_Command[b] == 'b' || _Command[b] == 'B'){leds[2] = !leds[2];}
		if (_Command[b] == 'w' || _Command[b] == 'W'){leds[0] = leds[1] = leds[2] = true;}
		if (_Command[b] == 'o' || _Command[b] == 'O'){leds[0] = leds[1] = leds[2] = false;}

		// Write to LED's
		analogWrite(Pins[0], (leds[0] == 1) ? 255 : 0);
		analogWrite(Pins[1], (leds[1] == 1) ? 255 : 0);
		analogWrite(Pins[2], (leds[2] == 1) ? 255 : 0);
	}
}

// simple random strobe
void strobe(){
	delay(SPEED);
	col[0] += random(-2, 2); if (col[0] < 0){col[0] = 0;}else if (col[0] > 255){col[0] = 255;}
	col[1] += random(-2, 2); if (col[1] < 0){col[1] = 0;}else if (col[1] > 255){col[1] = 255;}
	col[2] += random(-2, 2); if (col[2] < 0){col[2] = 0;}else if (col[2] > 255){col[2] = 255;}
	analogWrite(Pins[0], (byte)col[0]);
	analogWrite(Pins[1], (byte)col[1]);
	analogWrite(Pins[2], (byte)col[2]);
}

// Scrolling RGB leds
void scroll(){
	delay(SPEED);
	if (selected == 0){
		col[0]  = 0;
		col[1] -= 1;
		col[2] += 1;

		if (col[1] <= 0){ col[1] = 0; }
		if (col[2] >= 254){ col[2] = 255; selected = 1; }
 	}
 	if (selected == 1){
		col[0] += 1;
		col[1]  = 0;
		col[2] -= 1;

		if (col[2] <= 0){ col[2] = 0; }
		if (col[0] >= 254){ col[0] = 255; selected = 2; }
 	}
 	if (selected == 2){
		col[0] -= 1;
		col[1] += 1;
		col[2]  = 0;

		if (col[0] <= 0){ col[0] = 0; }
		if (col[1] >= 254){ col[1] = 255; selected = 0; }
 	}

	analogWrite(Pins[0], (byte)col[0]);
	analogWrite(Pins[1], (byte)col[1]);
	analogWrite(Pins[2], (byte)col[2]);
}

// Commands
void Commands(){
	char buffer[128];
	byte index = 0;
	
	while ( Serial.available() != 0 ){
		buffer[index] = Serial.read();
		index++;
		delay(10);
	}
	buffer[index] = '\0';

	if (buffer[0] == '\0'){return;}

	if (buffer[0] == 'r' || buffer[0] == 'g' || buffer[0] == 'b'){
		com = 0;
		char key = buffer[0];
		for (byte b = 0; b < index; b++){
			buffer[b] = buffer[b + 1];
		}

		int val = atoi(buffer);
		val = (val < 0) ? 0 : (val > 255) ? 255 : val;

		switch(key){
			case 'r':
			analogWrite(Pins[0], (byte)val);
			col[0] = (byte)val;
			break;
			case 'g':
			analogWrite(Pins[1], (byte)val);
			col[1] = (byte)val;
			break;
			case 'b':
			analogWrite(Pins[2], (byte)val);
			col[2] = (byte)val;
			break;
		}
	}

	if (buffer[0] == 't'){
		if (com == 3) {com = 2;}
		for (byte b = 0; b < index; b++){
			buffer[b] = buffer[b + 1];
		}

		SPEED = atoi(buffer);
		SPEED = (SPEED < 1) ? 1 : SPEED;
	}

	if (buffer[0] == 'w'){
		com = 0;
		analogWrite(Pins[0], 255);
		analogWrite(Pins[1], 255);
		analogWrite(Pins[2], 255);
		col[0] = col[1] = col[2] = 255;
	}

	if (buffer[0] == 'o'){
		com = 0;
		analogWrite(Pins[0], 0);
		analogWrite(Pins[1], 0);
		analogWrite(Pins[2], 0);
		col[0] = col[1] = col[2] = 0;
	}

	if (buffer[0] == 's'){
		com = 1;
	}

	if (buffer[0] == 'x'){
		com = 2;
		ScrollTime = 0.0f;
	}

	if (buffer[0] == 'z'){
		com = 3;
		for (byte b = 0; b < index; b++){
			CommandLoop[b] = buffer[b + 1];
		}
		CommandLoop[index + 1] = '\0';
	}
}

// Main Loop
void loop() {
	Commands();
	if (com == 1){ strobe(); }
	if (com == 2){ scroll(); }
	if (com == 3){ SpecialCommand(CommandLoop); }
}

