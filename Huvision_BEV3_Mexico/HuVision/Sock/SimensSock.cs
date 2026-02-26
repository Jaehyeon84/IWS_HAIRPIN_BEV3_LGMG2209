using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace HuVision.Sock
{
    class SimensSock : CommonSock
    {
        private libnodave.daveOSserialType fds;
        private libnodave.daveInterface di;
        private libnodave.daveConnection dc;
        private int rack = 0;
        private int slot = 1;
        private string name;
        private string ip = string.Empty;


        public SimensSock(string ip)
        {
            this.ip = ip;
            this.name = Guid.NewGuid().ToString();

            thread = new Thread(new ThreadStart(Connect));
            thread.IsBackground = true;
            thread.Start();
        }

        private void Connect()
        {
            while (true)
            {
                if (isStop)
                    break;

                if (!isConnected)
                {
                    try
                    {
                        fds.rfd = libnodave.openSocket(102, ip);
                        fds.wfd = fds.rfd;
                        if (fds.rfd > 0)
                        {
                            di = new libnodave.daveInterface(fds, name, 0, libnodave.daveProtoISOTCP, libnodave.daveSpeed1500k);
                            di.setTimeout(1000);

                            dc = new libnodave.daveConnection(di, 0, rack, slot);
                            if (0 == dc.connectPLC())
                            {
                                isCheckDisConnect = true;
                                isConnected = true;
                                OnConnected();
                            }
                            else
                            {
                                libnodave.closeSocket(fds.rfd);
                            }
                        }

                        if (!isConnected)
                        {
                            isConnected = false;
                            OnDisconnected();
                            isCheckDisConnect = false;
                        }
                    }
                    catch(Exception e)
                    {
                        OnDisconnected();
                        isCheckDisConnect = false;
                    }
                }
                Thread.Sleep(1000);
            }
        }

        public void CloseSimensSock()
        {
            try
            {
                isStop = true;
                isConnected = false;

                try
                {
                    if (thread != null)
                    {
                        thread.Abort();
                        thread = null;
                    }
                }
                catch { }

                dc.disconnectPLC();
                libnodave.closeSocket(fds.rfd);
            }
            catch { }
            finally
            {
                OnDisconnected();
            }
        }

        /// <summary>
        /// Read From PLC
        /// </summary>
        /// <param name="area">DB NUMBER</param>
        /// <param name="startAddr">DATA START ADDRESS : BYTE 단위</param>
        /// <param name="count">COUNT:1 = 2 BYTES</param>
        /// <returns></returns>
        public override byte[] Read(string area, int startAddr, int count)
        {
            if (!isConnected)
                return null;

            try
            {
                count = count * 2;
                byte[] data = new byte[count];

                int result = dc.readBytes(libnodave.daveDB, int.Parse(area), startAddr, count, data);

                if (result == 0)
                    return ByteSwap(data);
                else
                    return null;
            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, e);
                CloseSimensSock();
                return null;
            }
        }

        public override bool Write(string area, int startAddr, int count, byte[] data)
        {
            if (!isConnected)
                return false;

            try
            {
                count = data.Length;
                int res = dc.writeBytes(libnodave.daveDB, int.Parse(area), startAddr, count, ByteSwap(data));
                //int res = dc.writeBytes(libnodave.daveDB, area, startAddr, count, data);

                return (res == 0);
            }
            catch
            {
                //CloseSimensSock();
                return false;
            }

        }

        private List<byte> swapList = new List<byte>();
        private byte[] ByteSwap(byte[] data)
        {
            swapList.Clear();

            for(int i = 0; i < data.Length; i += 2)
            {
                swapList.Add(data[i + 1]);
                swapList.Add(data[i]);
            }

            return swapList.ToArray();
        }

    }
}
