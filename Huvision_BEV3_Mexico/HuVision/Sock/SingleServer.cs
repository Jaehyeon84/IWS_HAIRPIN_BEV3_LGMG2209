using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace HuVision.Sock
{
    class SingleServer
    {
        public event EventHandler Connected;
        public event EventHandler Disconnected;
        public event EventHandler DataReceived;
        public event EventHandler Stopped;

        public event EventHandler ErrorOccured;

        private Socket server;
        private Socket handler;

        public bool isServerStarted;
        public bool isClientConnected;

        private Thread acceptThread;
        private Thread workerThread;

        private int port;

        private byte[] recvData = new byte[8192];
        private List<byte> sendData = new List<byte>();

        private int bytesRec = 0;

        private enum PROTOCOL { STX = 0, COMMAND, SUB_COMMNAD, LENGTH, DATA, CHECKSUM, ETX }

        private const byte DEF_STX = 0x02;
        private const byte DEF_ETX = 0x03;

        private List<byte> recvList = new List<byte>();
        private List<byte> tmpRecvList = new List<byte>();

        private List<string> commnadList;

        public SingleServer()
        {
            commnadList = new List<string>() { "S", "STS", "A", "ALM", "R", "RCP", "H", "L1H", "L2H", "L3H", "L4H", "P", "LAX", "LAY", "LBX", "LBY", "LCD", "LTT" };
        }

        public void Open(int port)
        {
            if (isServerStarted)
                return;

            this.port = port;

            isServerStarted = true;

            acceptThread = new Thread(new System.Threading.ParameterizedThreadStart(ClientAcceptWorker));
            acceptThread.Start();

            //workerThread = new Thread(new System.Threading.ParameterizedThreadStart(Worker));
            //workerThread.Start();
        }

        private void ClientAcceptWorker(object obj)
        {
            if (server != null)
                Stop();

            isClientConnected = false;
            IPEndPoint ipEndPoint = new IPEndPoint(IPAddress.Any, this.port);
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            server.ReceiveTimeout = 20000;

            try
            {
                server.Bind(ipEndPoint);
                server.Listen(-1);

                while (isServerStarted)
                {
                    if(!isClientConnected)
                    {
                        handler = server.Accept();
                        isClientConnected = true;
                        OnConnected(handler);
                    }
                    
                    Thread.Sleep(1000);
                }
            }
            catch (Exception e)
            {
                OnErrorOccured("ClientAcceptWorker", e.Message);
            }
        }

        public byte[] SendAndReceive(string data)
        {
            if (!isClientConnected)
                return null;

            recvData = new byte[8192];
            bytesRec = 0;
            Send(data);
            return Receive();
        }

        public void Send(string data)
        {
            try
            {
                sendData.Clear();
                sendData = Encoding.Default.GetBytes(data).ToList();
                sendData.Insert(0, 0x02);
                sendData.Add(0x03);
                handler.Send(sendData.ToArray());
            }
            catch (SocketException se)
            {
                if (se.ErrorCode == 10054)   // 연결 끊김
                    Disconnect();
                else if (se.ErrorCode == 10060)   // TimeOut
                    OnErrorOccured("Send", "TimeOut");
                else
                    Disconnect();
            }
            catch (Exception e)
            {
                OnErrorOccured("Send", e.Message);
                Disconnect();
            }
        }

        public byte[] Receive()
        {
            try
            {
                while (true)
                {
                    if (handler.Available == 0 && bytesRec > 0)
                        return null;

                    bytesRec = handler.Receive(recvData);

                    if (bytesRec > 0)
                    {
                        //OnDataReceived(recvData.ToList().GetRange(0, bytesRec).ToArray());
                        //OnDataReceived(recvData.ToList().GetRange(0, bytesRec).ToArray());
                        return GetRecvDataArr(recvData.ToList().GetRange(0, bytesRec).ToArray());
                    }
                    else
                    {
                        return null;
                    }
                }
            }
            catch (SocketException se)
            {
                if (se.ErrorCode == 10054)   // 연결 끊김
                    Disconnect();
                else if (se.ErrorCode == 10060)   // TimeOut
                    OnErrorOccured("Receive", "TimeOut");
                else
                    Disconnect();

                return null;
            }
            catch (Exception e)
            {
                OnErrorOccured("Receive", e.Message);
                Disconnect();
                return null;
            }
        }

        public byte[] GetRecvDataArr(byte[] recvArr)
        {
            recvList.Clear();
            recvList.AddRange(recvArr);
            tmpRecvList.Clear();
            int indexOfStart = 0;

            while (true)
            {
                int startIndex = recvList.IndexOf(DEF_STX, indexOfStart);   // -1이 나올떄까지 계속 검사
                if (startIndex == -1)
                    break;
                else
                    indexOfStart = startIndex + 1;

                // 데이터 길이까지 데이터가 안나올경우 패스
                if ((startIndex + 9) >= recvList.Count)
                    continue;

                // 커맨드 검사
                string command = Encoding.Default.GetString(recvList.GetRange(startIndex + 1, 1).ToArray());
                if (!commnadList.Contains(command))
                {
                    OnErrorOccured("Command Not Search", Encoding.Default.GetString(recvList.ToArray()));
                    continue;
                }

                // 서브커맨드 검사
                string subCommand = Encoding.Default.GetString(recvList.GetRange(startIndex + 2, 3).ToArray());
                if (!commnadList.Contains(subCommand))
                {
                    OnErrorOccured("SubCommand Not Search", Encoding.Default.GetString(recvList.ToArray()));
                    continue;
                }

                // 데이터 길이 검사
                int dataLength = 0;
                try
                {
                    dataLength = int.Parse(Encoding.Default.GetString(recvList.GetRange(startIndex + 5, 4).ToArray()));
                }
                catch
                {
                    OnErrorOccured("Data Length Error", Encoding.Default.GetString(recvList.ToArray()));
                    recvList.Clear();
                    break;
                }

                // ETX 확인
                int endIndex = -1;
                endIndex = startIndex + 9 + dataLength + 2; // STX(1) + Command(1), SubCommand(3), Length(4) + data + checkSum

                // ETX 까지의 데이터를 못 받았을경우
                if (recvList.Count <= endIndex)
                    continue;

                if (recvList[endIndex] != DEF_ETX)
                {
                    OnErrorOccured("Not Search ETX", Encoding.Default.GetString(recvList.ToArray()));
                    break;
                }

                tmpRecvList.AddRange(recvList.GetRange(startIndex, (endIndex - startIndex) + 1).ToArray());
                recvList.RemoveRange(startIndex, (endIndex - startIndex) + 1);

                //recvList.Clear();
                //if (startIndex != 0)
                //    recvList.RemoveRange(0, startIndex);

                //indexOfStart = 0;
                // 프레임이 겹쳐 들어올 경우 보관하여 다중처리 하려 while문을 사용하였지만
                // 핑퐁 방식의 통신이기 때문에 다음 확인 시 후 프레임 확인을 위하여 break으로 종료
                break;
            }

            if (tmpRecvList.Count > 0)
                OnDataReceived(tmpRecvList.ToArray());

            return tmpRecvList.ToArray();
        }

        public void Disconnect()
        {
            isClientConnected = false;

            try { handler.Close(); }
            catch { }
            try { handler.Dispose(); }
            catch { }
            handler = null;

            OnDisconnected();
        }

        public void Stop()
        {
            isServerStarted = false;
            isClientConnected = false;

            try { server.Close(); }
            catch { }
            try { server.Dispose(); }
            catch { }
            server = null;

            try { handler.Close(); }
            catch { }
            try { handler.Dispose(); }
            catch { }
            handler = null;

            try { acceptThread.Abort(); }
            catch { }
            acceptThread = null;

            try { workerThread.Abort(); }
            catch { }
            workerThread = null;
        }

        private void OnConnected(Socket handler)
        {
            try
            {
                if (Connected != null)
                    Connected(handler, EventArgs.Empty);
            }
            catch (Exception ex)
            {
                OnErrorOccured("OnConnected()", ex.Message);
            }
        }

        private void OnDisconnected()
        {
            try
            {
                if (Disconnected != null)
                    Disconnected(null, EventArgs.Empty);
            }
            catch (Exception ex)
            {
                OnErrorOccured("OnDisconnected()", ex.Message);
            }
        }

        private void OnStopped()
        {
            try
            {
                isServerStarted = false;

                if (Stopped != null)
                    Stopped(null, EventArgs.Empty);
            }
            catch (Exception ex)
            {
                OnErrorOccured("OnStopped()", ex.Message);
            }
        }

        private void OnDataReceived(byte[] data)
        {
            try
            {
                if (DataReceived != null)
                    DataReceived(data, EventArgs.Empty);
            }
            catch (Exception ex)
            {
                OnErrorOccured("OnDataReceived()", ex.Message);
            }
        }

        private void OnErrorOccured(string sender, string msg)
        {
            try
            {
                if (ErrorOccured != null)
                    ErrorOccured(sender + ":" + msg, EventArgs.Empty);
            }
            catch (Exception ex) {  }
        }
    }
}
