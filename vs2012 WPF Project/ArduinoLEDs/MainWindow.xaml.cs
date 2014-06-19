// Luke Monaghan

using System;
using System.IO.Ports;
using System.Management;
using System.Windows;
using System.Windows.Input;
using System.Windows.Threading;

namespace ArduinoLEDs
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private SerialPort comport = new SerialPort();
        private DispatcherTimer dispatcherTimer;
        string com;

        public MainWindow()
        {
            InitializeComponent();

            // We write the information here, no real reason.
            tbTextInfo.Text = "Information:\nSliders control colours.";
            tbTextInfo.Text += "\nCustom Commands: case is side dependant.";
            tbTextInfo.Text += "\n->\tR,G and B will toggle that colour.";
            tbTextInfo.Text += "\n->\t0 to 9 = percent of 1 second to delay. Space and 0 = 100 percent. # = random.";
            tbTextInfo.Text += "\n->\tWw will set LED's to white.";
            tbTextInfo.Text += "\n->\tOo will turn off LED's.";
            tbTextInfo.Text += "\n->\tEnter to send and loop command.";

            // Detect an arduino com port. We could abstract this to the user, but no need. 
            // !!! PLEASE SUBMIT ISSUES IF YOUR DEVICES ISNT DETECTED !!!
            com = AutodetectArduinoPort();
            if (com != null)
            {
                comport.ReadTimeout = 500;
                comport.WriteTimeout = 500;
                comport.PortName = com;
                comport.BaudRate = 19200;
                comport.Open();
            }
            else
            {
                MessageBox.Show("No arduino COM port found. ");
            }

            //  DispatcherTimer setup
            dispatcherTimer = new DispatcherTimer();
            dispatcherTimer.Tick += new EventHandler(dispatcherTimer_Tick);
            dispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, 50);
            dispatcherTimer.Start();
        }

        private void dispatcherTimer_Tick(object sender, EventArgs e)
        {
            if (comport.IsOpen && comport.BytesToRead > 0)
            {
                String buffer = comport.ReadExisting();

                String sub;

                try
                {
                    sub = buffer.Substring(buffer.LastIndexOf('R') + 1, buffer.LastIndexOf('G') - (buffer.LastIndexOf('R') + 1));
                    RedSlider.Value = System.Convert.ToInt32(sub, 10);
                }
                catch { }
                try
                {
                    sub = buffer.Substring(buffer.LastIndexOf('G') + 1, buffer.LastIndexOf('B') - (buffer.LastIndexOf('G') + 1));
                    GreenSlider.Value = System.Convert.ToInt32(sub, 10);
                }
                catch { }
                try
                {
                    sub = buffer.Substring(buffer.LastIndexOf('B') + 1, buffer.LastIndexOf('r') - (buffer.LastIndexOf('B') + 1));
                    BlueSlider.Value = System.Convert.ToInt32(sub, 10);
                }
                catch { }

                try
                {
                    sub = buffer.Substring(buffer.LastIndexOf('r') + 1, buffer.LastIndexOf('g') - (buffer.LastIndexOf('r') + 1));
                    RedSliderL.Value = System.Convert.ToInt32(sub, 10);
                }
                catch { }
                try
                {
                    sub = buffer.Substring(buffer.LastIndexOf('g') + 1, buffer.LastIndexOf('b') - (buffer.LastIndexOf('g') + 1));
                    GreenSliderL.Value = System.Convert.ToInt32(sub, 10);
                }
                catch { }
                try
                {
                    sub = buffer.Substring(buffer.LastIndexOf('b') + 1, buffer.LastIndexOf('.') - (buffer.LastIndexOf('b') + 1));
                    BlueSliderL.Value = System.Convert.ToInt32(sub, 10);
                }
                catch { }
            }
        }

        // Arduino com port selector
        private string AutodetectArduinoPort()
        {
            ManagementObjectSearcher searcher = new ManagementObjectSearcher("root\\WMI", "SELECT * FROM MSSerial_PortName");
            foreach (ManagementObject queryObj in searcher.Get())
            {
                Console.WriteLine(queryObj["InstanceName"].ToString());
                if (queryObj["InstanceName"].ToString().Contains("Arduino") || queryObj["InstanceName"].ToString().Contains("FTDIBUS") || queryObj["InstanceName"].ToString().Contains("USB"))
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

        private void RedSliderL_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("R" + ((int)RedSliderL.Value).ToString());
            }
        }

        private void GreenSliderL_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("G" + ((int)GreenSliderL.Value).ToString());
            }
        }

        private void BlueSliderL_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("B" + ((int)BlueSliderL.Value).ToString());
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

        private void btnOffL_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("O");
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

        private void btnWhiteL_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (comport.IsOpen)
            {
                comport.WriteLine("W");
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
