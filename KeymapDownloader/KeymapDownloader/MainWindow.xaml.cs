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
        }

        /// <summary>
        /// 刷新HID列表
        /// </summary>
        void RefreshHIDList()
        {
            Devices.Items.Clear();

            var list = DeviceList.Local;
            var HidDeviceList = list.GetHidDevices(0x3d41, 0x1919);
            foreach (var item in HidDeviceList)
            {
                if (item.DevicePath.IndexOf("mi_02") != -1)
                {
                    Devices.Items.Add(item);
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

        private void Download_Click(object sender, RoutedEventArgs e)
        {
            var path = Path.Text;
            byte[] binary = new Byte[1024];

            HidStream hidStream = device.Open();

            if (!string.IsNullOrEmpty(path))
            {
                if (!File.Exists(path))
                {
                    lbl_status.Text = "配列文件不存在";
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
                        {
                            break;
                        }
                    }
                }
                // 第一个Byte为0x55代表启用此Keymap
                binary[0] = 0x55;
            }

            try
            {
                byte[] packet = new byte[60];

                for (int i = 0; i < (binary.Length / 60); i++)
                {
                    Array.Copy(binary, i * 60, packet, 0, 60);
                    SendPacket(hidStream, (uint)i, packet);
                }
            }
            catch (Exception exp)
            {
                lbl_status.Text = exp.Message;
            }

            lbl_status.Text = "完成";
        }

        void SendPacket(HidStream stream, uint id, byte[] data)
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
                ret_code = ret[1] == 0xc1;
            } while (!ret_code && retryCount-- > 0);

            if (retryCount == 0)
            {
                throw new Exception("发送重试次数达到上限");
            }
        }

        private void Devices_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            device = (HidDevice)Devices.SelectedItem;
        }
    }
}
