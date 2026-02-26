using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Sockets;
using System.Threading;
using System.Diagnostics;
using System.IO;
using System.Net.NetworkInformation;

namespace HubisCommonControl
{
    public class CommonSock
    {
        public delegate void DataReceiveEventHandler(byte[] data);
        public delegate void ConnectEventHandler();

        public event ConnectEventHandler Connected;
        public event ConnectEventHandler Disconnected;

        public bool IsConnected = false;

        public string IP { get; set; }
        public int PORT { get; set; }

        public Socket socket;
        public Thread thread;

        public bool isClose = false;

        public CommonSock()
        {

        }

        public CommonSock(string ip, int port)
        {
            this.IP = ip;
            this.PORT = port;

            thread = new Thread(new ThreadStart(ConnectThread));
            thread.IsBackground = true;
            thread.Start();
        }

        public void ConnectThread()
        {
            while (true)
            {
                if (isClose)
                    break;

                if (!IsConnected)
                {
                    try
                    {
                        CreateSock();
                        
                        socket.Connect(IP, PORT);
                        OnConnected();
                        LogManager.WriteLog(LogType.Information, "Connection is succeed." + "IP = [" + IP + "]" + " PORT = [" + PORT + "]");
                    }
                    catch (Exception ex)
                    {
                        //Trace.WriteLine(ex.Message);
                        OnDisconnected();
                        LogManager.WriteLog(LogType.Information, "Connection is failed" + ex.Message);
                    }
                }
                else // 소켓 끊어짐 감시
                {
                    CheckDisconnect();
                }
                Thread.Sleep(500);
            }
            Close();
        }

        private void CheckDisconnect()
        {
            if(socket.Connected == false)
            {
                OnDisconnected();
                return;
            }
            // Local PC의 IP 정보 획득
            IPGlobalProperties ipProperties = IPGlobalProperties.GetIPGlobalProperties();

            // TCP 상태정보를 정보를 얻어올 Tcp 구조체
            TcpConnectionInformation[] tcpConnections;

            // TCP 상태정보 획득
            tcpConnections = ipProperties.GetActiveTcpConnections().Where(x => x.LocalEndPoint.Equals(socket.LocalEndPoint)
            && x.RemoteEndPoint.Equals(socket.RemoteEndPoint)).ToArray();

            // 상태 확인하여 연결이 끊겨 있으면(Established가 아니면), Disconnect 처리
            if (tcpConnections != null && tcpConnections.Length > 0)
            {
                TcpState stateOfConnection = tcpConnections.First().State;
                if (stateOfConnection != TcpState.Established)
                {
                    OnDisconnected();
                }
            }
        }

        public void SetData(string ip, int port = 0)
        {
            this.IP = ip;
            this.PORT = port;
        }

        public bool CreateSock()
        {
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            socket.ReceiveTimeout = 30000;

            if (socket != null)
                return true;
            else
                return false;
        }

        private byte[] tmp = new byte[1];
        public void CheckConnect()
        {
            bool blockingState = socket.Blocking;
            try
            {
                socket.Blocking = false;
                socket.Send(tmp, 0, 0);
                IsConnected = true;
            }
            catch (SocketException e)
            {
                if (e.NativeErrorCode.Equals(10035))
                    IsConnected = true;
                else
                {
                    IsConnected = false;
                }
            }
            finally
            {
                socket.Blocking = blockingState;
            }
        }

        public byte[] SendAndReceive(byte[] data)
        {
            List<byte> lstData = new List<byte>();
            try
            {
                byte[] rcvData = new byte[1024];

                //전송 할 데이터가 존재할때만 Send
                if (data != null && data.Length > 0)
                {
                    int nRts = socket.Send(data);
                }

                int bytesRec = 0;
                while (true)
                {
                    if (socket.Available == 0 && bytesRec > 0)
                        break;

                    bytesRec = socket.Receive(rcvData);

                    if (bytesRec > 0)
                    {
                        lstData.AddRange(rcvData.ToList().GetRange(0, bytesRec));
                    }
                    else
                        break;
                }

            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, "Sand And Receive is Failed" + e.Message);
                return null;
            }

            if (lstData.Count == 0)
                return new byte[0];

            return lstData.ToArray();
        }

        public byte[] Receive()
        {
            List<byte> lstData = new List<byte>();
            try
            {
                byte[] rcvData = new byte[1024];

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
                // LogManager.WriteLog(LogType.Error, "Receive Error - " + e.Message);
                return null;
            }

            if (lstData.Count == 0)
                return new byte[0];

            return lstData.ToArray();
        }

        public void Send(byte[] data)
        {
            List<byte> lstData = new List<byte>();
            try
            {
                //전송 할 데이터가 존재할때만 Send
                if (data != null)
                {
                    int nRts = socket.Send(data);
                    LogManager.WriteLog(LogType.Information, "[Send] = " + Encoding.ASCII.GetString(data));
                }
            }
            catch (SocketException e)
            {
                LogManager.WriteLog(LogType.Error, e.Message);
            }
        }

        public void OnConnected()
        {
            try
            {
                IsConnected = true;

                if (Connected != null)
                    Connected();

            }
            catch (Exception e)
            {
                Close();
                Trace.WriteLine(e.Message);
            }
        }

        public void OnDisconnected()
        {
            try
            {
                if (Disconnected != null)
                    Disconnected();

                IsConnected = false;
            }

            catch (Exception e)
            {
                IsConnected = false;
                Trace.WriteLine(e.Message);
            }
        }

        public void Close()
        {
            try
            {
                try
                {
                    if (socket != null)
                    {
                        if (socket.Connected) socket.Shutdown(SocketShutdown.Both);
                    }
                }
                catch (SocketException ex)
                {
                    Trace.WriteLine(ex.Message);
                }
                try
                {
                    if (socket != null)
                        socket.Close();
                }
                catch (SocketException ex)
                {
                    Trace.WriteLine(ex.Message);
                }
                try
                {
                    if (socket != null)
                        socket.Dispose();
                }
                catch (SocketException ex)
                {
                    Trace.WriteLine(ex.Message);
                }

                socket = null;
            }
            catch (SocketException ex)
            {
                Trace.WriteLine(ex.Message);
            }
            finally
            {
                IsConnected = false;
                OnDisconnected();
            }
        }

        public virtual void ReConnect()
        {
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

        public virtual byte[] Read(string area, int startAddr, int count) { return null; }
        public virtual void Write(string area, int startAddr, int count, byte[] data) { }
    }
}
