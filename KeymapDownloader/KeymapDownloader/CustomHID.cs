using HidSharp;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace KeymapDownloader
{
    class CustomHID
    {
        public HidDevice Device { get; }

        public CustomHID(HidDevice device)
        {
            Device = device;
        }

        public override string ToString()
        {
            string result = "";

            string manufacturer = Device.GetManufacturer();
            string name = Device.GetProductName();
            string serial = Device.GetSerialNumber();
            int pid = Device.ProductID;
            int vid = Device.VendorID;

            result = String.Format("[{3:X4}:{4:X4}][{0}] {1} ({2})", manufacturer, name, serial, vid, pid);

            return result;
        }

        public static bool IsTarget(HidDevice device)
        {
            try
            {
                var report = device.GetReportDescriptor();
                if (report.ReportsUseID)
                {
                    foreach (var item in report.OutputReports)
                    {
                        if (item.ReportID == 0x3f && item.Length > 62)
                        {
                            var usages = item.GetAllUsages();
                            return true;
                        }
                    }
                }
            }
            catch { }

            return false;
        }
    }
}
