using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Net.NetworkInformation;


namespace HubisCommonControl
{
    class MelsecSock : CommonSock
    {
        private enum Area
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

        private enum ASCIICODE
        {
            GS = 0x1D,
            RS = 0x1E,
            EOT = 0x04
        }

        public MelsecSock(string ip, int port)
        {
            SetData(ip, port);

            thread = new Thread(new ThreadStart(Connect));
            thread.IsBackground = true;
            thread.Start();
        }

        public void Connect()
        {
            while (true)
            {
                if (!IsConnected)
                {
                    try
                    {
                        if (isClose)
                            break;

                        if (CreateSock())
                        {
                            socket.Connect(IP, PORT);

                            OnConnected();
                            LogManager.WriteLog(LogType.Information, "Connected is succeed. = IP[" + IP + "], Port = [" + PORT + "]");
                        }
                    }
                    catch (Exception e)
                    {
                        OnDisconnected();
                        LogManager.WriteLog(LogType.Information, "Connected is failed." + e.Message);
                    }
                }
                else // 소켓 끊어짐 감시
                {
                    CheckDisconnect();
                }

                Thread.Sleep(500);
            }
        }

        private void CheckDisconnect()
        {
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

        /// <summary>
        /// 
        /// </summary>
        /// <param name="area">영역</param>
        /// <param name="startAddr">시작 Address</param>
        /// <param name="count">WORD Cnt</param>
        /// <returns></returns>
        public override byte[] Read(string area, int startAddr, int count)
        {
            byte[] recvData = null;
            try
            {
                if (!IsConnected)
                    return null;

                Area a = (Area)Enum.Parse(typeof(Area), area);
                byte[] plcReqData = ReadFromPLC(a, startAddr, count);

                recvData = SendAndReceive(plcReqData);
                int recvDataLen = recvData.Length - 11;
                
                if (recvData.Length <= 0)
                    return null;

                int recvedDataLenInfo = BitConverter.ToUInt16(recvData, 7) - 2;
                if (!(recvDataLen == recvedDataLenInfo && recvedDataLenInfo == (count * 2)))
                    return null;

                if (recvData != null)
                    recvData = recvData.ToList().GetRange(11, recvData.Length - 11).ToArray();
                
                return recvData;
            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, e.Message);
                Close();
                return null;
            }
        }
                
        public override void Write(string area, int startAddr, int count, byte[] data)
        {
            try
            {
                Area a = (Area)Enum.Parse(typeof(Area), area);
                byte[] tempData = WriteToPLC(a, startAddr, count, data);
                socket.Send(tempData);
                int result = socket.Receive(new byte[100]);
                if (result == -1)
                    MessageBox.Show("Write Fail : " + result);
            }
            catch (Exception e)
            {
                //LogManager.WriteLog(LogType.Error, e.Message);
                //LogManager.WriteLog(LogType.Information, "PLC Write");
                LogManager.WriteLog(LogType.Error, "[PLC Write Error]" + e.Message);
                Close();
            }
        }

        private byte[] ReadFromPLC(Area area, int StartAddr, int ReadCnt)
        {
            byte[] SendFrame = new byte[21];

            SendFrame[0] = 0x50; //서브헤더
            SendFrame[1] = 0x00; //서브헤더
            SendFrame[2] = 0x00; //네트워크 번호
            SendFrame[3] = 0xFF; //&HFF 'PLC 번호
            SendFrame[4] = 0xFF; //요구상대 모듈 IO번호_L
            SendFrame[5] = 0x03; //요구상대 모듈 IO번호_H
            SendFrame[6] = 0x00; //요구상대 모듈 국번호
            SendFrame[7] = 0x0C; //요구 데이터 길이_L (12바이트)
            SendFrame[8] = 0x00; //요구 데이터 길이_H (12바이트)
            SendFrame[9] = 0x10; //CPU 감시 타이머_L
            SendFrame[10] = 0x00; //CPU 감시 타이머_H
            //Word Read :::: Command = 0401
            SendFrame[11] = 0x01; //커맨드_L
            SendFrame[12] = 0x04; //커맨드_H
            SendFrame[13] = 0x00; //서브 커맨드_L
            SendFrame[14] = 0x00; //서브 커맨드_H
            //읽기 주소 설정
            SendFrame[15] = (byte)(StartAddr % 256); //선두 디바이스_L
            SendFrame[16] = (byte)(StartAddr / 256); //선두 디바이스_H
            SendFrame[17] = 0x00;//선두 디바이스
            SendFrame[18] = (byte)area;
            SendFrame[19] = (byte)(ReadCnt % 256); //데이터 개수_L
            SendFrame[20] = (byte)(ReadCnt / 256); //데이터 개수_H

            return SendFrame;
        }

        private byte[] WriteToPLC(Area area, int StartAddr, int WordCnt, byte[] wData)
        {

            byte[] SendFrame = new byte[21 + WordCnt * 2];
            SendFrame[0] = 0x50;//'서브헤더
            SendFrame[1] = 0x00;//'서브헤더
            SendFrame[2] = 0x00;//'네트워크 번호
            SendFrame[3] = 0xFF;//FF'PLC 번호
            SendFrame[4] = 0xFF;//'요구상대 모듈 IO번호_L
            SendFrame[5] = 0x03;//'요구상대 모듈 IO번호_H
            SendFrame[6] = 0x00;//'요구상대 모듈 국번호
            SendFrame[7] = (byte)(12 + (WordCnt * 2));//'요구 데이터 길이_L (12바이트)
            SendFrame[8] = 0x00;//'요구 데이터 길이_H (12바이트)
            SendFrame[9] = 0x10;//'CPU 감시 타이머_L
            SendFrame[10] = 0x00;//'CPU 감시 타이머_H
            //'Word Write :::: Command = 1401
            SendFrame[11] = 0x01;//'커맨드_L
            SendFrame[12] = 0x14;//'커맨드_H
            SendFrame[13] = 0x00;//'서브 커맨드_L
            SendFrame[14] = 0x00;//'서브 커맨드_H
            //'읽기 주소 설정
            SendFrame[15] = (byte)(StartAddr % 256);//'선두 디바이스_L
            SendFrame[16] = (byte)(StartAddr / 256);//'선두 디바이스_H
            SendFrame[17] = 0x00;//'선두 디바이스
            SendFrame[18] = (byte)area;
            SendFrame[19] = (byte)(WordCnt % 256);//'데이터 개수_L
            SendFrame[20] = (byte)(WordCnt / 256);//'데이터 개수_H
            //SendFrame.SetValue(wData, 21);
            int offset = 21;
            for (int i = 0; i < wData.Length; i++)
                SendFrame[offset + i] = wData[i];

            return SendFrame;
        }        
    }
}
