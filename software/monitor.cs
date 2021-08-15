using System;
using System.Threading;
using LibUsbDotNet;
using LibUsbDotNet.Main;
using LibreHardwareMonitor.Hardware;

namespace monitor
{
    class Program
    {
        const int VendorId = 0x1466;
        const int ProductId = 0x4034;
        const int UpdatePeriodMs = 1000;
        const int DeviceConnectPeriodMs = 5000;
        static UsbSetupPacket DisplayStatsCmd = new UsbSetupPacket {
            RequestType = 0x21,
            Request = 0x80,
            Value = 0,
            Index = 0,
            Length = 8
        };

        static void Main(string[] args) {
            // Initialize computer
            Computer computer = new Computer {
                IsCpuEnabled = true,
                IsGpuEnabled = true,
                IsMemoryEnabled = true,
                IsMotherboardEnabled = false,
                IsControllerEnabled = false,
                IsNetworkEnabled = false,
                IsStorageEnabled = false
            };
            computer.Open();

            // Find hardware
            IHardware cpu = null;
            IHardware gpu = null;
            IHardware ram = null;
            foreach (IHardware hardware in computer.Hardware) {
                switch(hardware.HardwareType) {
                    case HardwareType.GpuNvidia:
                    case HardwareType.GpuAmd:
                        gpu = hardware;
                        break;
                    case HardwareType.Cpu:
                        cpu = hardware;
                        break;
                    case HardwareType.Memory:
                        ram = hardware;
                        break;
                }
                if (cpu != null && gpu != null && ram != null)
                    break;
            }

            // Collect load, memory, and temperature sensors from hardware
            ISensor[] sensors = new ISensor[6];
            foreach (ISensor sensor in cpu.Sensors) {
                if (sensor.SensorType == SensorType.Load && sensor.Name == "CPU Total")
                    sensors[0] = sensor;
                else if (sensor.SensorType == SensorType.Temperature && sensor.Name == "Core (Tctl/Tdie)")
                    sensors[2] = sensor;
            }
            foreach (ISensor sensor in ram.Sensors) {
                if (sensor.SensorType == SensorType.Data && sensor.Name == "Memory Used")
                    sensors[1] = sensor;
            }
            foreach (ISensor sensor in gpu.Sensors) {
                if (sensor.SensorType == SensorType.Load && sensor.Name == "GPU Core")
                    sensors[3] = sensor;
                else if (sensor.SensorType == SensorType.SmallData && sensor.Name == "GPU Memory Used")
                    sensors[4] = sensor;
                else if (sensor.SensorType == SensorType.Temperature && sensor.Name == "GPU Core")
                    sensors[5] = sensor;
            }

            // Infinite loop to connnect to usb device and write stats
            UsbDevice device = null;
            int ret = 0;
            int stat = 0;
            byte[] stats = {0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x00, 0x00};
            while (true) {
                if (device != null) {
                    // Update sensors
                    cpu.Update();
                    gpu.Update();
                    ram.Update();
                    for (int i = 0; i < 6; i++) {
                        if (i == 1) // Digits must be stored in byte, so scale X.X to XX
                            stat = (int) (sensors[i].Value * 10);
                        else if (i == 4) // Digits must be stored in byte, so scale XXXX to XX
                            stat = (int) (sensors[i].Value / 100);
                        else
                            stat = (int) sensors[i].Value;
                        if (stat >= 100)
                            stat = 99;
                        if (stat < 10 && i != 1 && i != 4)
                            stats[i] = (byte) (0xA0 | stat); // 0xA represent blank digit
                        else
                            stats[i] = (byte)  (((stat / 10) << 4) | (stat % 10)); // Convert stat to BCD
                    }
                    // Write to device, attempt reconnect if failed to write 8 bytes
                    device.ControlTransfer(ref DisplayStatsCmd, stats, stats.Length, out ret);
                    if (ret == 0) {
                        device.Close();
                        device = FindDevice();
                    }
                } else
                    device = FindDevice();
                Thread.Sleep(device == null ? DeviceConnectPeriodMs : UpdatePeriodMs);
            }
        }

        private static UsbDevice FindDevice() {
            // Loop through devices and return device with matching vendor and product id
            UsbDevice device;
            foreach (UsbRegistry reg in UsbDevice.AllDevices) {
                if (reg.Open(out device)) {
                    if (device.UsbRegistryInfo.Vid == VendorId && device.UsbRegistryInfo.Pid == ProductId)
                        return device;
                }
            }
            return null;
        }
    }
}