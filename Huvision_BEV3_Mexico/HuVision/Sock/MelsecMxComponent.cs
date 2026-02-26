using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HuVision.Sock
{
    public class MelsecMxComponent
    {
        public enum Area
        {
            D = 0xA8,
            W = 0xB4,
            r = 0xAF,
            ZR = 0xB0,
            X = 0x9C,
            Y = 0x9D,
            M = 0x90,
            L = 0x92,
            s = 0x98,
            B = 0xA0,
            f = 0x93,
            TS = 0xC1,
            TN = 0xC2,
            CS = 0xC4,
            CC = 0xC3,
            CN = 0xC5
        }

        private Encoding encoding = Encoding.GetEncoding(949);

        public delegate void ConnectEventHandler();

        public event ConnectEventHandler Connected;
        public event ConnectEventHandler Disconnected;

        private ActUtlTypeLib.ActUtlType actUtlType;

        private Thread thread;
        private bool isConnected = false;
        private bool isStop = false;

        private int plcNumber = 0;

        public void Init(int plcNumber)
        {
            this.plcNumber = plcNumber;

            actUtlType = new ActUtlTypeLib.ActUtlType();
            actUtlType.ActLogicalStationNumber = plcNumber;

            thread = new Thread(new ThreadStart(ConnectWorker));
            thread.IsBackground = true;
            thread.Start();
        }

        private void ConnectWorker()
        {
            while (true)
            {
                if (isStop)
                    break;

                if (!isConnected)
                {
                    try
                    {
                        if (actUtlType.Open() == 0)
                            OnConnected();
                    }
                    catch (Exception e)
                    {
                        LogManager.WriteLog(LogType.Error, e);
                        Close();
                    }
                }

                Thread.Sleep(1000);
            }
        }

        public byte[] Read(string area, string startAddr, int count, string dataCode = "B")
        {
            string device = area + startAddr.ToString();

            int[] datas = new int[count];
            int result = actUtlType.ReadDeviceBlock(device, count, out datas[0]);

            if (result != 0)
                return null;

            List<byte> dataList = new List<byte>();
            foreach (int data in datas)
                dataList.AddRange(BitConverter.GetBytes((UInt16)data));

            return dataList.ToArray();
        }

        public string ASCII_GetString(byte[] data, int index, int length)
        {
            return encoding.GetString(data);// index, length);//.Trim('\0');
        }

        public byte[] ASCII_ConvertToBinary(string strValue)
        {
            List<byte> buf = new List<byte>();
            byte temp = 0x00;
            for (int i = 0; i < strValue.Length / 2; i++)
            {
                if (i % 2 == 0)
                {
                    temp = byte.Parse(strValue.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber);
                }
                else
                {
                    buf.Add(byte.Parse(strValue.Substring(i * 2, 2), System.Globalization.NumberStyles.HexNumber));
                    buf.Add(temp);
                }
            }

            return buf.ToArray();
        }

        public bool Write(string area, string startAddr, byte[] data, string dataCode)
        {
            //count = data.Length / 2;
            int count = (int)Math.Ceiling(data.Length / 2.0);

            if (data.Length % 2 != 0)
            {
                byte[] tmpByte = new byte[data.Length + 1];
                Array.Copy(data, tmpByte, data.Length);

                data = new byte[tmpByte.Length];
                Array.Copy(tmpByte, data, tmpByte.Length);
            }

            string device = area + startAddr.ToString();

            int[] datas = new int[count];
            for (int i = 0; i < count; i++)
                datas[i] = (int)BitConverter.ToInt16(data, i * 2);

            int result = actUtlType.WriteDeviceBlock(device, count, ref datas[0]);

            return result == 0;
        }

        protected void OnConnected()
        {
            try
            {
                isConnected = true;

                if (Connected != null)
                    Connected();
            }
            catch (Exception e)
            {
                Close();
                LogManager.WriteLog(LogType.Error, e.Message);
            }
        }

        protected void OnDisconnected()
        {
            try
            {
                if (Disconnected != null)
                    Disconnected();

                isConnected = false;
            }
            catch (Exception e)
            {
                isConnected = false;
                LogManager.WriteLog(LogType.Error, e.Message);
            }
        }

        public void Close()
        {
            try
            {
                actUtlType.Close();
            }
            catch { }
            finally
            {
                OnDisconnected();
            }
        }

        public void Exit()
        {
            isStop = true;

            try
            {
                if (thread != null)
                {
                    thread.Abort();
                    thread = null;
                }
            }
            catch { }

            Close();
        }
    }
}
