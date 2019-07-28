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
            setStatusText($"配列下载器 {System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString()}");
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

            try
            {
                byte[] packet = new byte[60];
                var count = binary.Length / 60;
                for (int i = 0; i < count; i++)
                {
                    Array.Copy(binary, i * 60, packet, 0, 60);
                    setStatusText($"{i + 1}/{count}");

                    if (SendPacket(hidStream, (uint)i, packet))
                        break;
                }
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
            byte[] send = new byte[63];

            send[0] = 0x3f;
            send[1] = (byte)id;
            Array.Copy(data, 0, send, 2, data.Length);

            int retryCount = 5;

            bool ret_code;
            do
            {
                stream.Write(send);
                var ret = stream.Read();
                var code = ret[1];

                // 0x10: 校验失败
                // 0x11: 接收成功
                // 0x12: 全部发送完毕
                if (code == 0x12)
                    return true;

                ret_code = code == 0x11;
            } while (!ret_code && retryCount-- > 0);

            if (retryCount <= 0)
            {
                throw new Exception("发送重试次数达到上限");
            }
            return false;
        }

        private void Devices_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            device = ((CustomHID)Devices.SelectedItem)?.Device;
        }
    }
}
