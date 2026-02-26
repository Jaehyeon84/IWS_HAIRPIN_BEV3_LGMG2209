using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Threading;

namespace HuVision.Sock
{
    public class CommonSock
    {
        public delegate void DataReceiveEventHandler(byte[] data);
        public delegate void ConnectEventHandler();

        public event ConnectEventHandler Connected;
        public event ConnectEventHandler Disconnected;

        protected bool isConnected = false;
        protected bool isStop = false;

        public string IP { get; set; }
        public int PORT { get; set; }
        public int STATION_NO { get; set; }

        protected Socket socket;
        

        protected bool isCheckDisConnect = true;

        //public virtual void Start() { }
        //public virtual void ConnectWorker() { }

        protected void SetData(string ip, int port = 0)
        {
            this.IP = ip;
            this.PORT = port;
        }

        public void StartConnectThread()
        {
            Thread thread = new Thread(new ThreadStart(Connect));
            thread.IsBackground = true;
            thread.Start();
        }

        private void Connect()
        {
            Thread.Sleep(2000);

            while (true)
            {
                if (isStop)
                    break;

                if (!isConnected)
                {
                    try
                    {
                        socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                        socket.Connect(IP, PORT);
                        socket.ReceiveTimeout = 500;
                        socket.SendTimeout = 500;
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

            Close();
        }

        //protected void CreateSock()
        //{
        //    socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        //}

        public byte[] SendAndReceive(byte[] data)
        {
            List<byte> lstData = new List<byte>();
            try
            {
                byte[] rcvData = new byte[1024];

                socket.Send(data);
                int bytesRec = 0;
                while (true)
                {
                    if (socket.Available == 0 && bytesRec > 0)
                        break;

                    bytesRec = socket.Receive(rcvData);

                    if (bytesRec > 0)
                        lstData.AddRange(rcvData.ToList().GetRange(0, bytesRec));
                    else
                        break;
                }

            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, "CommonSock_SendAndReceive: " + e);
                Close();

                return null;
            }

            if (lstData.Count == 0)
                return new byte[0];

            return lstData.ToArray();
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
                if(isCheckDisConnect)
                {
                    if (Disconnected != null)
                        Disconnected();
                }

                isConnected = false;
            }
            catch (Exception e)
            {
                isConnected = false;
            }
        }

        public void Close()
        {
            try
            {
                if(socket != null)
                {
                    try { if (socket.Connected) socket.Shutdown(SocketShutdown.Both); }
                    catch { }
                    try { socket.Close(); }
                    catch { }
                    try { socket.Dispose(); }
                    catch { }

                    socket = null;
                }
            }
            catch {}
            finally
            {
                isConnected = false;
                OnDisconnected();
            }
        }

        public void Exit()
        {
            isStop = true;

            //try
            //{
            //    if (thread != null)
            //    {
            //        thread.Abort();
            //        thread = null;
            //    }
            //}
            //catch { }

            //Close();
        }

        public virtual byte[] Read(string area, int startAddr, int count) { return null; }
        public virtual bool Write(string area, int startAddr, int count, byte[] data) { return false; }
    }
}
