using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using HidSharp;
using IntelHexFormatReader;
using IntelHexFormatReader.Model;
using System.Threading;

namespace KeymapDownloader
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow
    {
        HidDevice device;

        public MainWindow()
        {
            InitializeComponent();
            RefreshHIDList();
            SetCopyright();
        }

        void SetCopyright()
        {
            setStatusText($"配列下载器 {System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString()} | 请使用新版配置工具进行配置");
        }

        /// <summary>
        /// 刷新HID列表
        /// </summary>
        void RefreshHIDList()
        {
            Devices.Items.Clear();

            var list = DeviceList.Local;
            var HidDeviceList = list.GetHidDevices();
            foreach (var item in HidDeviceList)
            {
                if (CustomHID.IsTarget(item))
                {
                    Devices.Items.Add(new CustomHID(item));
                }
            }
            if (Devices.Items.Count > 0) Devices.SelectedIndex = 0;
        }

        private void refresh_Click(object sender, RoutedEventArgs e)
        {
            RefreshHIDList();
        }

        private void browse_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog ofd = new Microsoft.Win32.OpenFileDialog();
            ofd.DefaultExt = ".hex";
            ofd.Filter = "HEX的配列文件|*.hex;*.eep|BIN格式的配列文件|*.bin";
            if (ofd.ShowDialog() == true)
            {
                Path.Text = ofd.FileName;
            }
        }

        bool checkSum(byte[] data)
        {
            const int checksum_offset = 0x13;
            int checksum = (data[checksum_offset]) + (data[checksum_offset + 1] << 8);

            int calc_sum = 0xFEED;
            for (int i = checksum_offset + 2; i < data.Length - 1; i += 2)
            {
                var c = (data[i]) + (data[i + 1] << 8);
                if (c == 0xFFFF) break; // 数据末端
                calc_sum += c;
                calc_sum %= 0x10000;
            }

            return calc_sum == checksum;
        }

        private void Download_Click(object sender, RoutedEventArgs e)
        {
            var path = Path.Text;
            Thread thread = new Thread(() => { DownloadKeymap(path); });
            thread.Start();
        }

        private void DownloadKeymap(string path)
        {
            byte[] binary = new Byte[1024];

            HidStream hidStream = device.Open();

            if (!string.IsNullOrEmpty(path))
            {
                if (!File.Exists(path))
                {
                    setStatusText("配列文件不存在");
                    return;
                }

                if (path.EndsWith("bin", StringComparison.CurrentCultureIgnoreCase))
                {
                    using (var stream = File.Open(path, FileMode.Open))
                    {
                        stream.Read(binary, 0, 1024);
                    }
                }
                else
                {
                    HexFileReader reader = new HexFileReader(path, 1024);
                    MemoryBlock memoryRepresentation = reader.Parse();

                    int index = 0;
                    foreach (var item in memoryRepresentation.Cells)
                    {
                        binary[index++] = item.Value;
                        if (index == 1024)
                            break;
                    }
                }

                if (!checkSum(binary))
                {
                    setStatusText("Keymap校验不通过");
                    return;
                }
            }

            const int keymap_offset = 0x55;

            try
            {
                byte[] packet = new byte[56];
                var count = (binary.Length - keymap_offset) / 56;
                // Keymap
                for (int i = 0; i < count; i++)
                {
                    Array.Copy(binary, i * 56 + keymap_offset, packet, 0, 56);
                    setStatusText($"{i + 1}/{count}");

                    if (SendPacket(hidStream, (uint)i, packet))
                        break;
                }

                // Fn
                for (int i = 0; i < 64; i += 56)
                {
                    Array.Copy(binary, i + 0x15, packet, 0, i < 64 ? 56 : i - 56);
                    if (SendPacketFn(hidStream, (uint)i/56, packet))
                        break;
                }

                // Write to storage
                SendCommand(hidStream, 0x3E, new byte[] { 0xFF });

                setStatusText("下载完毕");
            }
            catch (Exception exp)
            {
                setStatusText(exp.Message);
            }
        }

        void setStatusText(string msg)
        {
            Dispatcher.Invoke(() => { lbl_status.Text = msg; });
        }

        /// <summary>
        /// 发送数据包
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="id"></param>
        /// <param name="data"></param>
        /// <returns>是否为最后一个</returns>
        bool SendPacket(HidStream stream, uint id, byte[] data)
        {
            byte[] send = new byte[64];

            send[0] = 0x3f;
            send[1] = 0x33; // CMD: Write keymap
            send[2] = (byte)(data.Length + 1);
            send[3] = (byte)id;

            Array.Copy(data, 0, send, 4, data.Length);

            int retryCount = 5;

            bool ret_code;
            do
            {
                stream.Write(send);
                byte[] ret = new byte[64];
                stream.Read(ret);
                var code = ret[1];

                // 0x04: 写入完毕
                if (code == 0x04)
                    return true;

                ret_code = (code == 0x00);
            } while (!ret_code && retryCount-- > 0);

            if (retryCount <= 0)
            {
                throw new Exception("发送重试次数达到上限");
            }
            return false;
        }

        /// <summary>
        /// 发送数据包
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="id"></param>
        /// <param name="data"></param>
        /// <returns>是否为最后一个</returns>
        bool SendPacketFn(HidStream stream, uint id, byte[] data)
        {
            byte[] send = new byte[64];

            send[0] = 0x3f;
            send[1] = 0x34; // CMD: Write keymap
            send[2] = (byte)(data.Length + 1);
            send[3] = (byte)id;

            Array.Copy(data, 0, send, 4, data.Length);

            int retryCount = 5;

            bool ret_code;
            do
            {
                stream.Write(send);
                byte[] ret = new byte[64];
                stream.Read(ret);
                var code = ret[1];

                // 0x04: 写入完毕
                if (code == 0x04)
                    return true;

                ret_code = (code == 0x00);
            } while (!ret_code && retryCount-- > 0);

            if (retryCount <= 0)
            {
                throw new Exception("发送重试次数达到上限");
            }
            return false;
        }

        byte[] SendCommand(HidStream stream, uint cmd, byte[] data)
        {
            byte[] send = new byte[64];

            send[0] = 0x3f;
            send[1] = (byte)cmd;
            send[2] = (byte)data.Length;

            if (data.Length > 0)
            {
                Array.Copy(data, 0, send, 3, data.Length);
            }

            stream.Write(send);
            byte[] ret = new byte[64];
            stream.Read(ret);
            byte[] real;
            if (ret[2] == 0)
            {
                real = new byte[1];
            } else
            {
                real = new byte[2 + ret[2]];
            }
            Array.Copy(ret, 1, real, 0, real.Length);

            return real;
        }

        private void Devices_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            device = ((CustomHID)Devices.SelectedItem)?.Device;
            try
            {
                HidStream hidStream = device.Open();
                var ret = SendCommand(hidStream, 0x20, new byte[0]);
                System.Console.WriteLine("%02x", ret);
            }
            catch (Exception exp)
            {
                setStatusText(exp.Message);
            }
        }
    }
}
