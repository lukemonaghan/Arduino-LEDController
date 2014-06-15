// Luke Monaghan

using System;
using System.IO.Ports;
using System.Management;
using System.Windows;
using System.Windows.Input;

namespace ArduinoLEDs
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private SerialPort comport = new SerialPort();
        string com;

        public MainWindow()
        {
            InitializeComponent();
            // We write the information here, no real reason.
            tbTextInfo.Text = "Information:\nSliders control colours.";
            tbTextInfo.Text += "\nCustom Commands:";
            tbTextInfo.Text += "\n->\tR,G and B will toggle that colour. (not case senstive).";
            tbTextInfo.Text += "\n->\t0 to 9 = percent of 1 second to delay. Space and 0 = 100 percent. # = random.";
            tbTextInfo.Text += "\n->\tW will set LED's to white.";
            tbTextInfo.Text += "\n->\tO will turn off LED's.";
            tbTextInfo.Text += "\n->\tEnter to send and run command. ";
            // Detect an arduino com port. We could abstract this to the user, but no need. 
            // !!! PLEASE SUBMIT ISSUES IF YOUR DEVICES ISNT DETECTED !!!
            com = AutodetectArduinoPort();
            if (com != null)
            {
                comport.PortName = com;
                comport.BaudRate = 9600;
                comport.Open();
            }
            else
            {
                MessageBox.Show("No arduino COM port found. ");
                Close();
            }
        }

        // Arduino com port selector
        private string AutodetectArduinoPort()
        {
            ManagementObjectSearcher searcher = new ManagementObjectSearcher("root\\WMI", "SELECT * FROM MSSerial_PortName");
            foreach (ManagementObject queryObj in searcher.Get())
            {
                Console.WriteLine(queryObj["InstanceName"].ToString());
                if (queryObj["InstanceName"].ToString().Contains("FTDIBUS") || queryObj["InstanceName"].ToString().Contains("USB"))
                {
                    return queryObj["PortName"].ToString();
                }
            }
            return null;
        }

        // Simple colour sliders
        private void RedSlider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("r" + ((int)RedSlider.Value).ToString());
            }
        }

        private void GreenSlider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("g" + ((int)GreenSlider.Value).ToString());
            }
        }

        private void BlueSlider_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("b" + ((int)BlueSlider.Value).ToString());
            }
        }

        // OFF
        private void btnOff_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("o");
            }
        }

        // ON / WHITE
        private void btnWhite_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("w");
            }
        }

        // Scroll Command
        private void btnScroll_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("x");
            }
        }

        // Strobe Command
        private void btnStrobe_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("s");
            }
        }

        // Delay Speed Control
        private void sldSpeed_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("t" + ((int)sldSpeed.Value).ToString());
            }
        }

        // Send Custom Command
        private void tbSpecial_PreviewKeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                comport.WriteLine("z" + tbSpecial.Text.ToString());
            }
        }
    }
}
