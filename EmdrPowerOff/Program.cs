using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Management;
using System.Text;
using System.Threading.Tasks;

namespace EmdrPowerOff
{
    class Program
    {
        private static ManagementEventWatcher comInsertWatcher;
        private static ManagementEventWatcher comRemoveWatcher;
        private static string[] oldSerialPorts = { };

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
            lock (oldSerialPorts)
            {
                var allSerialPorts = SerialPort.GetPortNames();
                var serialPorts = allSerialPorts.Except(oldSerialPorts).ToArray();
                oldSerialPorts = allSerialPorts;

                foreach (string serialPort in serialPorts)
                {
                    bool isArduino = IsArduinoDevice(serialPort, out string deviceId, out string deviceName);
                    if (isArduino)
                        Console.WriteLine($"arduino detected at {deviceId}");
                }
            }
        }

        private static void ComRemoveEventHandler()
        {
            lock (oldSerialPorts)
            {
                var allSerialPorts = SerialPort.GetPortNames();
                var serialPorts = oldSerialPorts.Except(allSerialPorts);
                oldSerialPorts = allSerialPorts;

                foreach (string serialPort in serialPorts)
                    Console.WriteLine($"removed {serialPort}");
            }
        }

        private static bool IsArduinoDevice(string serialPort, out string deviceId, out string deviceName)
        {
            deviceId = null;
            deviceName = null;

            ManagementObjectSearcher mos = new ManagementObjectSearcher(
                    $"Select * from Win32_SerialPort where DeviceId = '{serialPort}'");
            foreach (var mo in mos.Get())
            {
                deviceId = mo["DeviceID"].ToString();
                deviceName = mo["Name"].ToString();
                Console.WriteLine($"inserted {deviceId}: {deviceName}");
                return deviceName.Contains("Arduino");
            }

            Console.WriteLine($"inserted {serialPort}: ?");

            return false;
        }
    }
}
