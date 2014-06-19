// Luke Monaghan
// ArduinoLEDs

#include "Math.h"

// Pinouts
// 6,5,3 are LEDs
// 8 is GND
byte Pins[7] = {6,5,3,7,11,10,9};

// Speed
short SPEED = 10;

// LED Colours
short col[6] = {0,0,0,0,0,0};

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
	pinMode(Pins[4], OUTPUT);
	pinMode(Pins[5], OUTPUT);
	pinMode(Pins[6], OUTPUT);
	
	//Serial
	Serial.begin(19200);

	// boot Sequence
	SpecialCommand("rB4BrgR4RgbG4Gb4Wo4Ow4o");

	// Now scroll
	com = 2;
}

void SpecialCommand(char _Command[]){
	bool leds[6] = {0,0,0,0,0,0};

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
		if (_Command[b] == 'r'){leds[0] = !leds[0];}
		if (_Command[b] == 'g'){leds[1] = !leds[1];}
		if (_Command[b] == 'b'){leds[2] = !leds[2];}

		if (_Command[b] == 'R'){leds[3] = !leds[3];}
		if (_Command[b] == 'G'){leds[4] = !leds[4];}
		if (_Command[b] == 'B'){leds[5] = !leds[5];}

		if (_Command[b] == 'w'){leds[0] = leds[1] = leds[2] = true;}
		if (_Command[b] == 'o'){leds[0] = leds[1] = leds[2] = false;}

		if (_Command[b] == 'W'){leds[3] = leds[4] = leds[5] = true;}
		if (_Command[b] == 'O'){leds[3] = leds[4] = leds[5] = false;}

		// Write to LED's
		col[0] = (leds[0] == 1) ? 255 : 0;
		col[1] = (leds[1] == 1) ? 255 : 0;
		col[2] = (leds[2] == 1) ? 255 : 0;

		col[3] = (leds[3] == 1) ? 255 : 0;
		col[4] = (leds[4] == 1) ? 255 : 0;
		col[5] = (leds[5] == 1) ? 255 : 0;

		analogWrite(Pins[0], (byte)col[0]);
		analogWrite(Pins[1], (byte)col[1]);
		analogWrite(Pins[2], (byte)col[2]);	

		analogWrite(Pins[4], (byte)col[3]);
		analogWrite(Pins[5], (byte)col[4]);
		analogWrite(Pins[6], (byte)col[5]);
		SendData();
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

	col[3] += random(-2, 2); if (col[3] < 0){col[3] = 0;}else if (col[3] > 255){col[3] = 255;}
	col[4] += random(-2, 2); if (col[4] < 0){col[4] = 0;}else if (col[4] > 255){col[4] = 255;}
	col[5] += random(-2, 2); if (col[5] < 0){col[5] = 0;}else if (col[5] > 255){col[5] = 255;}
	analogWrite(Pins[4], (byte)col[3]);
	analogWrite(Pins[5], (byte)col[4]);
	analogWrite(Pins[6], (byte)col[5]);
	SendData();
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

 	col[3] = col[0];
 	col[4] = col[1];
 	col[5] = col[2];
	analogWrite(Pins[4], (byte)col[3]);
	analogWrite(Pins[5], (byte)col[4]);
	analogWrite(Pins[6], (byte)col[5]);
	SendData();
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

	if (buffer[0] == 'R' || buffer[0] == 'G' || buffer[0] == 'B'){
		com = 0;
		char key = buffer[0];
		for (byte b = 0; b < index; b++){
			buffer[b] = buffer[b + 1];
		}

		int val = atoi(buffer);
		val = (val < 0) ? 0 : (val > 255) ? 255 : val;

		switch(key){
			case 'R':
			analogWrite(Pins[4], (byte)val);
			col[3] = (byte)val;
			break;
			case 'G':
			analogWrite(Pins[5], (byte)val);
			col[4] = (byte)val;
			break;
			case 'B':
			analogWrite(Pins[6], (byte)val);
			col[5] = (byte)val;
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

	if (buffer[0] == 'W'){
		com = 0;
		analogWrite(Pins[4], 255);
		analogWrite(Pins[5], 255);
		analogWrite(Pins[6], 255);
		col[3] = col[4] = col[5] = 255;
	}

	if (buffer[0] == 'o'){
		com = 0;
		analogWrite(Pins[0], 0);
		analogWrite(Pins[1], 0);
		analogWrite(Pins[2], 0);
		col[0] = col[1] = col[2] = 0;
	}

		if (buffer[0] == 'O'){
		com = 0;
		analogWrite(Pins[4], 0);
		analogWrite(Pins[5], 0);
		analogWrite(Pins[6], 0);
		col[3] = col[4] = col[5] = 0;
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
	SendData();
}

void SendData(){
	String VAL = "R" + String(col[0]);
	VAL += "G" + String(col[1]);
	VAL += "B" + String(col[2]);

	VAL += "r" + String(col[3]);
	VAL += "g" + String(col[4]);
	VAL += "b" + String(col[5]);
	VAL += ".";

	char BUFFER[VAL.length()+1];
	VAL.toCharArray(BUFFER,VAL.length()+1);
	Serial.write(BUFFER);
}

// Main Loop
void loop() {
	Commands();
	if (com == 1){ strobe(); }
	if (com == 2){ scroll(); }
	if (com == 3){ SpecialCommand(CommandLoop); }
}

