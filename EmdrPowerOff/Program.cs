using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace EmdrPowerOff
{
    class Program
    {
        private static ManagementEventWatcher comInsertWatcher;
        private static ManagementEventWatcher comRemoveWatcher;
        private static string[] oldPortNames = { };
        private static string arduinoPortName;
        private static SerialPort arduinoSerialPort = new SerialPort();
        private static Thread readThread = new Thread(Read);
        static bool _continue;

        static void Main(string[] args)
        {
            StartComWatcher();
            ComInsertEventHandler();
            Console.WriteLine("press enter do exit...");
            Console.ReadLine();
        }

        private static void StartComWatcher()
        {
            var comInsertedQuery = new WqlEventQuery(
                "SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 2");
            var comRemovalQuery = new WqlEventQuery(
                "SELECT * FROM Win32_DeviceChangeEvent WHERE EventType = 3");

            comInsertWatcher = new ManagementEventWatcher(comInsertedQuery);
            comRemoveWatcher = new ManagementEventWatcher(comRemovalQuery);

            comInsertWatcher.EventArrived +=
                (object sender, EventArrivedEventArgs e) => ComInsertEventHandler();
            comRemoveWatcher.EventArrived +=
                (object sender, EventArrivedEventArgs e) => ComRemoveEventHandler();

            comInsertWatcher.Start();
            comRemoveWatcher.Start();

            // anulowanie subskrypcji podłączenia/odłączenie urządzenia usb
            AppDomain.CurrentDomain.ProcessExit += (object sender, EventArgs e) =>
            {
                comInsertWatcher.Stop();
                comRemoveWatcher.Stop();
                Console.WriteLine("com watcher stopped");
            };

            Console.WriteLine("com watcher started");
        }

        private static void ComInsertEventHandler()
        {
            lock (oldPortNames)
            {
                var allPortNames = SerialPort.GetPortNames();
                var portNames = allPortNames.Except(oldPortNames).ToArray();
                oldPortNames = allPortNames;

                foreach (string portName in portNames)
                {
                    if (IsArduinoDevice(portName, out string deviceName))
                        OpenArduinoSerialPort(portName, deviceName);
                }
            }
        }

        private static void ComRemoveEventHandler()
        {
            lock (oldPortNames)
            {
                var allPortNames = SerialPort.GetPortNames();
                var portNames = oldPortNames.Except(allPortNames);
                oldPortNames = allPortNames;

                foreach (string portName in portNames)
                    if (portName == arduinoPortName)
                        CloseArduinoSerialPort(portName);
                    else
                        Console.WriteLine($"removed {portName}");
            }
        }

        private static bool IsArduinoDevice(string portName, out string deviceName)
        {
            deviceName = null;

            ManagementObjectSearcher mos = new ManagementObjectSearcher(
                    $"Select * from Win32_SerialPort where DeviceId = '{portName}'");
            foreach (var mo in mos.Get())
            {
                deviceName = mo["Name"].ToString();
                Console.WriteLine($"inserted {portName}: {deviceName}");
                bool isArduino = deviceName.Contains("Arduino");
                if (isArduino)
                    Console.WriteLine("arduino inserted");
                return isArduino;
            }

            Console.WriteLine($"inserted {portName}: ?");
            return false;
        }

        private static void OpenArduinoSerialPort(string portName, string deviceName)
        {
            arduinoPortName = portName;
            arduinoSerialPort.PortName = portName;
            arduinoSerialPort.BaudRate = 115200;
            arduinoSerialPort.ReadTimeout = 500;
            arduinoSerialPort.WriteTimeout = 500;

            //arduinoSerialPort.DataReceived += ArduinoSerialPort_DataReceived;
            arduinoSerialPort.Open();
            _continue = true;
            readThread.Start();
            Console.WriteLine("arduino opened");

            const string hello = "?";
            arduinoSerialPort.Write(hello);
            Console.WriteLine($"arduino < {hello}");

            //string recv = arduinoSerialPort.ReadExisting();
            //Console.WriteLine($"received from arduino: {recv}");
        }

        private static void ArduinoSerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //Console.WriteLine($"received from arduino: {0}", arduinoSerialPort.ReadExisting());
            //arduinoSerialPort.Close();
        }

        private static void CloseArduinoSerialPort(string portName)
        {
            Console.WriteLine($"arduino removed");
            arduinoPortName = null;
        }

        public static void Read()
        {
            while (_continue)
            {
                try
                {
                    //string message = arduinoSerialPort.ReadLine();
                    string recv = arduinoSerialPort.ReadLine();
                    Console.WriteLine($"arduino > {recv}");
                    switch (recv)
                    {
                        case "emdr":
                            Console.WriteLine("emdr ready");
                            break;
                        case "suspend":
                            Console.WriteLine("suspending...");
                            SetSuspendState(false, true, true);
                            Console.WriteLine("awoken");
                            break;
                        default:
                            break;
                    }
                }
                catch (TimeoutException) { }
            }
        }

        [DllImport("PowrProf.dll", CharSet = CharSet.Auto, ExactSpelling = true)]
        public static extern bool SetSuspendState(bool hiberate, bool forceCritical, bool disableWakeEvent);
    }
}
