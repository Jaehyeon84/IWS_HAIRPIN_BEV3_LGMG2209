using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading;
using HubisCommonControl;  // 휴비스 공통 클래스 사용하기 위해 선언(Log, Socket 등)
using MIS.Common;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace MIS.Communication
{
    class GMESSocket : CommonSock
    {
        // 멤버변수  -----------------------------------------------
        // Ini 파일 클래스
        protected ConfigINI m_cConfig = new ConfigINI();

        // JSON 클래스
        protected JSONReader m_clsJSON = new JSONReader();

        // EAYT_R 수신 정보(장비의 MES 정보, EQPID 등)
        protected EAYT_R m_EQPInfo = new EAYT_R();

        // EPDS 수신 정보(수신된 스테이터스 정보)
        protected EPDS m_ProductData = new EPDS(); // 수신 데이터 저장
        protected EPDS m_ProductDataJigA = new EPDS(); // 수신 데이터 분리 저장 변수(JIG A)
        protected EPDS m_ProductDataJigB = new EPDS(); // 수신 데이터 분리 저장 변수(JIG B)
        protected EPDS m_ProductDataOldJigA = new EPDS(); // 수신 데이터 분리 저장 변수(JIG A)
        protected EPDS m_ProductDataOldJigB = new EPDS(); // 수신 데이터 분리 저장 변수(JIG B)

        // EERS_R 수신정보
        protected EERS_R m_EERS_Ack = new EERS_R();

        // EIDS_R 수신정보
        protected EIDS_R m_EIDS_Ack = new EIDS_R();

        // PLC Thread 함수
        protected Thread m_MESThread;
        protected bool m_bThreadRun = false;

        // 수신 메세지 버퍼
        protected List<string> m_listRcvBuffer = new List<string>();

        // MES -> EQP 수신 Flag
        public bool m_bGMES_EAYT_R { get; set; }
        public bool m_bGMES_ELNT { get; set; }
        public bool m_bGMES_EDTS { get; set; }
        public bool m_bGMES_EEMR_R { get; set; }
        public bool m_bGMES_EIDS_R { get; set; }
        public bool m_bGMES_EERS_R { get; set; }
        public bool m_bGMES_EPDS { get; set; }

        // EQP -> MES 송신 Flag
        public bool m_bEQP_EAYT { get; set; }
        public bool m_bEQP_ELNT_R { get; set; }
        public bool m_bEQP_EDTS_R { get; set; }
        public bool m_bEQP_EEMR { get; set; }
        public bool m_bEQP_EIDS { get; set; }
        public bool m_bEQP_EERS { get; set; }
        public bool m_bEQP_EPDS_R { get; set; }

        public bool m_bLinkTest = false;
        public bool m_bLinkTestAck = false;

        public struct SYSTEMTIME
        {
            public ushort wYear;
            public ushort wMonth;
            public ushort wDayOfWeek;
            public ushort wDay;
            public ushort wHour;
            public ushort wMinute;
            public ushort wSecond;
            public ushort wMilliseconds;
        }
        public struct JIG_MESSTATUS
        {
            public bool bEQP_EERS;
            public bool bGMES_EERS_R;
            public bool bEQP_EPDS_R;
            public bool bGMES_EPDS;
            public bool bEQP_EIDS;
            public bool bGMES_EIDS_R;
            public bool bProductInJig;
        }

        JIG_MESSTATUS m_stStatusJigA;
        JIG_MESSTATUS m_stStatusJigB;

        SYSTEMTIME m_stGMESTime;

        [DllImport("kernel32.dll")]
        public extern static bool SetLocalTime(ref SYSTEMTIME time);

        // 멤버함수  -----------------------------------------------
        // 생성자
        public GMESSocket()
        {
            // Get Ini Info & Set Info
            string strPath = Application.StartupPath + "\\MISConfig.ini";
            string strExe = Application.StartupPath + "\\";
            m_cConfig.InIPath = strPath;
            m_cConfig.ReadConfig();
            m_cConfig.WriteConfig();

            this.IP = m_cConfig.GMES_IP;
            this.PORT = m_cConfig.GMES_Port;

            thread = new Thread(new ThreadStart(ConnectThread));
            thread.IsBackground = true;
            thread.Start();
        }

        public void UpdateLinkTest()
        {
            m_bLinkTest = !m_bLinkTest;
        }

        public void UpdateLinkTestAck()
        {
            m_bLinkTestAck = !m_bLinkTestAck;
        }

        // 초기화 함수
        public void Intialize()
        {
            // Clear All Flag
            ClearAllFlag();

            // Thrad Start
            MESThreadStart();
        }

        // 종료 함수
        public void Terminate()
        {
            MESThreadStop();
        }

        public void ClearAllFlag()
        {
            // Link Test Flag
            m_bLinkTest = false;
            m_bLinkTestAck = false;


            // MES -> EQP 수신 플래그
            m_bGMES_EAYT_R = false;
            m_bGMES_ELNT = false;
            m_bGMES_EDTS = false;
            m_bGMES_EEMR_R = false;
            m_bGMES_EIDS_R = false;
            m_bGMES_EERS_R = false;
            m_bGMES_EPDS = false;

            // EQP -> MES 송신 Flag
            m_bEQP_EAYT = false;
            m_bEQP_ELNT_R = false;
            m_bEQP_EDTS_R = false;
            m_bEQP_EEMR = false;
            m_bEQP_EIDS = false;
            m_bEQP_EERS = false;
            m_bEQP_EPDS_R = false;

            // Jig별 상태 정보
            m_stStatusJigA.bEQP_EERS = false;
            m_stStatusJigA.bGMES_EERS_R = false;
            m_stStatusJigA.bEQP_EPDS_R = false;
            m_stStatusJigA.bGMES_EPDS = false;
            m_stStatusJigA.bEQP_EIDS = false;
            m_stStatusJigA.bGMES_EIDS_R = false;
            
            m_stStatusJigB.bEQP_EERS = false;
            m_stStatusJigB.bGMES_EERS_R = false;
            m_stStatusJigB.bEQP_EPDS_R = false;
            m_stStatusJigB.bGMES_EPDS = false;
            m_stStatusJigB.bEQP_EIDS = false;
            m_stStatusJigB.bGMES_EIDS_R = false;
            
        }

        public void ClearJigStatus(int nJigNo)
        {
            if(nJigNo == 0) // Jig A
            {
                m_stStatusJigA.bEQP_EERS = false;
                m_stStatusJigA.bGMES_EERS_R = false;
                m_stStatusJigA.bEQP_EPDS_R = false;
                m_stStatusJigA.bGMES_EPDS = false;
                m_stStatusJigA.bEQP_EIDS = false;
                m_stStatusJigA.bGMES_EIDS_R = false;
                m_ProductDataJigA.SetID = "";

            }
            else   // Jig B
            {
                m_stStatusJigB.bEQP_EERS = false;
                m_stStatusJigB.bGMES_EERS_R = false;
                m_stStatusJigB.bEQP_EPDS_R = false;
                m_stStatusJigB.bGMES_EPDS = false;
                m_stStatusJigB.bEQP_EIDS = false;
                m_stStatusJigB.bGMES_EIDS_R = false;
                m_ProductDataJigB.SetID = "";
            }
        }

        // 쓰레드 시작 함수(MES Send / Receive)
        protected void MESThreadStart()
        {
            m_MESThread = new Thread(new ThreadStart(MESThreadProcess));
            m_bThreadRun = true;
            m_MESThread.Start();
        }

        // 쓰레드 종료 함수
        protected void MESThreadStop()
        {
            m_bThreadRun = false;
            m_MESThread.Abort();
            m_MESThread = null;
        }

        // MES->PC 수신
        public readonly char STX = Convert.ToChar(0x02);
        public readonly char ETX = Convert.ToChar(0x03);

        protected void GMESReceiveProc()
        {
            if(IsConnected == false)
            {
                return;
            }

            //lock(m_listRcvBuffer)
            //{
                byte[] RcvData = null;
                try
                {
                    // 데이터 수신
                    RcvData = Receive();

                    if(RcvData != null && RcvData.Length > 0)
                    {
                        // 수신 메세지를 문자열로 변환
                        string strMsg = Encoding.ASCII.GetString(RcvData);

                        // 수신 메세지를 STX 기준으로 나눔.
                        string[] strSplitMsg = strMsg.Split(STX);

                        // 나눠진 메세지를 수신 리스트에 추가
                        for(int i=0; i<strSplitMsg.Count(); i++)
                        {
                            if (strSplitMsg[i].Length > 1)
                            {
                                // ETX 위치 찾기
                                int nEnd = strSplitMsg[i].IndexOf(ETX);
                                if (nEnd > 0)
                                {
                                    m_listRcvBuffer.Add(strSplitMsg[i].Substring(0, nEnd));
                                }
                                else
                                {
                                    m_listRcvBuffer.Add(strSplitMsg[i]);
                                }
                                
                            }
                        }
                    }
                }
                catch (Exception e)
                {
                    LogManager.WriteLog(LogType.Error, e.Message);
                }
            //}
        }

        // 쓰레드 함수
        protected void MESThreadProcess()
        {
            while (m_bThreadRun)
            {
                GMESReceiveProc();

//                GMESSendProc();
                Thread.Sleep(1);
            }
        }

        // Read / Write Inferface 함수 -----------------------------------
        protected string GetReceiveMessage()
        {
            string strRcv = string.Empty;

            if(m_listRcvBuffer.Count() == 0)
            {
                return strRcv;
            }

            strRcv = m_listRcvBuffer[0];
            LogManager.WriteLog(LogType.Information, "[Recv] = " + strRcv);

            m_listRcvBuffer.RemoveAt(0);

            return strRcv;
        }

        // 수신된 메세지 가져오는 함수
        // 메세지를 하나씩 가져와 임시파일로 저장 한 뒤,
        // Parsing하여 JSONData List에 추가후, 임시파일은 삭제
        // Parsing할 때, 어떤 메세지인지 MSGID Return
        public string GetReceiveJson()
        {
            string strMSGID = string.Empty;

            // Get Rcv Message
            string strRcvData = GetReceiveMessage();

            if(strRcvData.Length > 0)
            {
                // Write Tmp file
                string strTmpFile = Application.StartupPath + "\\recv_tmp.json";
                m_clsJSON.WriteFile(strRcvData, strTmpFile);

                // Parsing And Rename
                strMSGID = m_clsJSON.ReadJsonFileAndRename(strTmpFile);

            }

            return strMSGID;
        }

        // 수신된 메세지를 처리하는 함수 -------------------------------------
        // 메세지별로 처리하는 방식이 다르므로, 별도의 함수 따로 작성.
        public void RcvProcEAYT_R()
        {
            // 수신 Flag ON
            m_bGMES_EAYT_R = true;

            // 송신 Flag Off
            m_bEQP_EAYT = false;

            // Equipment Info 저장
            if (m_clsJSON.GetJSONDatas().Count > 0)
            {
                foreach (JSONData Tmp in m_clsJSON.GetJSONDatas())
                {
                    switch (Tmp.NodeName)
                    {
                        case "EQPID":   m_EQPInfo.EQPID = Tmp.Value; break;
                        case "EQPNAME": m_EQPInfo.EQPName = Tmp.Value; break;
                        case "PROCID": m_EQPInfo.ProcID = Tmp.Value; break;
                        case "ORGID": m_EQPInfo.ORGID = Tmp.Value; break;
                        case "LINEID": m_EQPInfo.LineID = Tmp.Value; break;
                    }
                }
            }
        }

        public void RcvProcELNT()
        {
            // 수신 Flag On
            m_bGMES_ELNT = true;

            // Update Link Test
            UpdateLinkTest();

            // Send ACK
            SendELNT_ACK(GetEquipInfo().EQPID);
        }

        public void RcvProcEDTS()
        {
            // 수신 Flag On
            m_bGMES_EDTS = true;

            // 수신데이터 확인
            int nACK = (int)eACK.ACK_NOTACCEPTED;
            string strTimeValue = string.Empty;
            if (m_clsJSON.GetJSONDatas().Count > 0)
            {
                foreach (JSONData Tmp in m_clsJSON.GetJSONDatas())
                {

                    switch (Tmp.NodeName)
                    {
                        case "EQPID":
                            break;
                        case "TIME":
                            strTimeValue = Tmp.Value;
                            nACK = (int)eACK.ACK_ACCEPTED;
                            break;
                    }
                }
            }

            // 시간 변경
            if (nACK == 0)
            {
                // SetLocalTime
                m_stGMESTime.wYear = Convert.ToUInt16(strTimeValue.Substring(0, 4));
                m_stGMESTime.wMonth = Convert.ToUInt16(strTimeValue.Substring(4, 2));
                m_stGMESTime.wDay = Convert.ToUInt16(strTimeValue.Substring(6, 2));
                m_stGMESTime.wHour = Convert.ToUInt16(strTimeValue.Substring(8, 2));
                m_stGMESTime.wMinute = Convert.ToUInt16(strTimeValue.Substring(10, 2));
                m_stGMESTime.wSecond = Convert.ToUInt16(strTimeValue.Substring(12, 2));
                SetLocalTime(ref m_stGMESTime);
            }

            // Send ACK
            SendEDTS_ACK(GetEquipInfo().EQPID, nACK);
        }

        public void RcvProcEEMR_R()
        {
            // 수신 Flag ON
            m_bGMES_EEMR_R = true;

            // 송신 Flag Off
            m_bEQP_EEMR = false;
        }

        public int RcvProcEIDS_R()
        {
            // 수신 Flag ON
            m_bGMES_EIDS_R = true;

            // 송신 Flag Off
            m_bEQP_EIDS = false;

            // 수신데이터 확인
            if (m_clsJSON.GetJSONDatas().Count > 0)
            {
                foreach (JSONData Tmp in m_clsJSON.GetJSONDatas())
                {
                    switch (Tmp.NodeName)
                    {
                        case "SETID":
                            m_EIDS_Ack.SETID = Tmp.Value;
                            break;
                        case "ACK":
                            m_EIDS_Ack.ACK = Tmp.Value;
                            break;
                        case "REASON":
                            m_EIDS_Ack.REASON = Tmp.Value;
                            break;
                    }
                }
            }

            // Set Send / Receive Flag(Jig A)
            if (
                (m_ProductDataJigA.SetID.Length > 0 && m_EIDS_Ack.SETID.Equals(m_ProductDataJigA.SetID))
                || (m_ProductDataOldJigA.SetID.Length > 0 && m_EIDS_Ack.SETID.Equals(m_ProductDataOldJigA.SetID))
               )
            {
                m_stStatusJigA.bGMES_EIDS_R = true;
                m_stStatusJigA.bEQP_EIDS = false;

                if(m_EIDS_Ack.ACK == "0")
                {
                    m_ProductDataJigA.SetID = "";
                }

                return 0;
            }
            // Set Send / Receive Flag(Jig B)
            else if (
                     (m_ProductDataJigB.SetID.Length > 0 && m_EIDS_Ack.SETID.Equals(m_ProductDataJigB.SetID))
                     || (m_ProductDataOldJigB.SetID.Length > 0 && m_EIDS_Ack.SETID.Equals(m_ProductDataOldJigB.SetID))
                    )
            {
                
                m_stStatusJigB.bGMES_EIDS_R = true;
                m_stStatusJigB.bEQP_EIDS = false;

                if (m_EIDS_Ack.ACK == "0")
                {
                    m_ProductDataJigB.SetID = "";
                }

                return 1;
            }

            LogManager.WriteLog(LogType.Error, "[EIDS_R Don't know Jig No] ACK : " + m_EIDS_Ack.SETID + ", Jig A : " + m_ProductDataJigA.SetID + ", Jig B : " + m_ProductDataJigB.SetID);

            return -1;
        }

        public void RcvProcEERS_R()
        {
            // 수신 Flag ON
            m_bGMES_EERS_R = true;

            // 송신 Flag Off
            m_bEQP_EERS = false;

            // 수신데이터 확인
            string strSETID = string.Empty;
            if (m_clsJSON.GetJSONDatas().Count > 0)
            {
                foreach (JSONData Tmp in m_clsJSON.GetJSONDatas())
                {
                    switch (Tmp.NodeName)
                    {
                        case "EQPID":
                            m_EERS_Ack.EQPID = Tmp.Value;
                            break;
                        case "CEID":
                            m_EERS_Ack.CEID = Tmp.Value;
                            break;
                        case "RPTID":
                            m_EERS_Ack.RPTID = Tmp.Value;
                            break;
                        case "ACK":
                            m_EERS_Ack.ACK = Tmp.Value;
                            break;
                        case "REASON":
                            m_EERS_Ack.REASON = Tmp.Value;
                            break;
                    }
                }
            }

            // Set Flag
            if(m_EERS_Ack.nJigType == eJigType.JigA)
            {
                m_stStatusJigA.bEQP_EERS = false;
                m_stStatusJigA.bGMES_EERS_R = true;
            }
            else
            {
                m_stStatusJigB.bEQP_EERS = false;
                m_stStatusJigB.bGMES_EERS_R = true;
            }

        }

        public int RcvProcEPDS()
        {
            // 수신 Flag ON
            m_bGMES_EPDS = true;
            m_bEQP_EPDS_R = false;

            // ProductData 저장
            if (m_clsJSON.GetJSONDatas().Count > 0)
            {
                int nSubItemIndex = 0;
                bool bFind = false;
                foreach (JSONData Tmp in m_clsJSON.GetJSONDatas())
                {
                    switch (Tmp.NodeName)
                    {
                        case "TID": m_ProductData.TID = Tmp.Value; break;
                        case "WOID": m_ProductData.WorkID = Tmp.Value; break;
                        case "ORGID": m_ProductData.ORGID = Tmp.Value; break;
                        case "LINEID": m_ProductData.LineID = Tmp.Value; break;
                        //case "EQPID": ProcData.EQPID = strEQPID; break;
                        //case "PROCID": ProcData.ProcID = Tmp.Value; break;
                        case "SETID": m_ProductData.SetID = Tmp.Value; break;
                    }

                    if (Tmp.ParaentNode == "MODEL")
                    {
                        switch (Tmp.NodeName)
                        {
                            case "ID": m_ProductData.model_ID = Tmp.Value; break;
                            case "NAME": m_ProductData.model_Name = Tmp.Value; break;
                            case "SUFFIX": m_ProductData.model_Suffix = Tmp.Value; break;
                        }
                    }
                    else if (Tmp.ParaentNode == "SUBITEM_LIST")
                    {
                        if (Tmp.NodeName == "NAME")
                        {
                            ProductSubItem Item = new ProductSubItem();
                            m_ProductData.LsSubItem.Add(Item);
                            m_ProductData.LsSubItem[nSubItemIndex].strName = Tmp.Value;

                            if (Tmp.Value == "MODEL_INFO")
                            {
                                bFind = true;
                            }
                        }
                        else if (Tmp.NodeName == "VALUE")
                        {
                            m_ProductData.LsSubItem[nSubItemIndex].strValue = Tmp.Value;
                            nSubItemIndex++;

                            if (bFind)
                            {
                                m_ProductData.strStatorType = Tmp.Value;

                                bFind = false;
                            }
                        }
                    }
                }
            }

            // Send Ack
            SendEPDS_ACK(m_EQPInfo.EQPID);

            // Set ProductData
            if(m_ProductData.SetID == m_ProductDataJigA.SetID)
            {
                // Check In Jig
                if(m_stStatusJigA.bProductInJig == true)
                {
                    // Move Current Data to Old
                    SetOldProductData(0, m_ProductDataJigA);

                    // (Logging)
                    LogManager.WriteLog(LogType.Information, "Jig A : Copy Product Data to Old Buffer");
                }
                else
                {
                    // Clear Old Data
                    EPDS stClear = new EPDS();
                    SetOldProductData(0, stClear);
                }

                // Set Product Data
                SetProductData(0, m_ProductData);

                // Set Flag
                m_stStatusJigA.bGMES_EPDS = false;
                m_stStatusJigA.bEQP_EPDS_R = true;

                return 0;
            }
            else if(m_ProductData.SetID == m_ProductDataJigB.SetID)
            {
                // Check In Jig
                if (m_stStatusJigB.bProductInJig == true)
                {
                    // Move Current Data to Old
                    SetOldProductData(1, m_ProductDataJigB);

                    // (Logging)
                    LogManager.WriteLog(LogType.Information, "Jig B : Copy Product Data to Old Buffer");
                }
                else
                {
                    // Clear Old Data
                    EPDS stClear = new EPDS();
                    SetOldProductData(1, stClear);
                }

                // Set Product Data
                SetProductData(1, m_ProductData);

                // Set Flag
                m_stStatusJigB.bGMES_EPDS = false;
                m_stStatusJigB.bEQP_EPDS_R = true;

                return 1;
            }
            else
            {
                LogManager.WriteLog(LogType.Error, "[EPDS Don't know Jig No] EPDS : " + m_ProductData.SetID + ", Jig A : " + m_ProductDataJigA.SetID + ", Jig B : " + m_ProductDataJigB.SetID);
                return -1;
            }
        }

        public void SetProductData(int nJigNo, EPDS stProductData)
        {
            if (nJigNo == 0) // Jig A
            {
                m_ProductDataJigA.LineID        = stProductData.LineID;
                m_ProductDataJigA.model_ID      = stProductData.model_ID;
                m_ProductDataJigA.model_Name    = stProductData.model_Name;
                m_ProductDataJigA.model_Suffix  = stProductData.model_Suffix;
                m_ProductDataJigA.ORGID         = stProductData.ORGID;
                m_ProductDataJigA.TID           = stProductData.TID;
                m_ProductDataJigA.WorkID        = stProductData.WorkID;
                m_ProductDataJigA.LsSubItem     = stProductData.LsSubItem;
                m_ProductDataJigA.strStatorType = stProductData.strStatorType;
            }
            else if (nJigNo == 1) // Jig B
            {
                m_ProductDataJigB.LineID        = stProductData.LineID;
                m_ProductDataJigB.model_ID      = stProductData.model_ID;
                m_ProductDataJigB.model_Name    = stProductData.model_Name;
                m_ProductDataJigB.model_Suffix  = stProductData.model_Suffix;
                m_ProductDataJigB.ORGID         = stProductData.ORGID;
                m_ProductDataJigB.TID           = stProductData.TID;
                m_ProductDataJigB.WorkID        = stProductData.WorkID;
                m_ProductDataJigB.LsSubItem     = stProductData.LsSubItem;
                m_ProductDataJigB.strStatorType = stProductData.strStatorType;
            }
        }

        public void SetProductDataExceptSubItem(int nJigNo, EPDS stProductData)
        {
            if (nJigNo == 0) // Jig A
            {
                m_ProductDataJigA.LineID        = stProductData.LineID;
                m_ProductDataJigA.model_ID      = stProductData.model_ID;
                m_ProductDataJigA.model_Name    = stProductData.model_Name;
                m_ProductDataJigA.model_Suffix  = stProductData.model_Suffix;
                m_ProductDataJigA.ORGID         = stProductData.ORGID;
                m_ProductDataJigA.TID           = stProductData.TID;
                m_ProductDataJigA.WorkID        = stProductData.WorkID;
//                m_ProductDataJigA.LsSubItem     = m_ProductData.LsSubItem;
                m_ProductDataJigA.strStatorType = stProductData.strStatorType;
            }
            else if (nJigNo == 1) // Jig B
            {
                m_ProductDataJigB.LineID        = stProductData.LineID;
                m_ProductDataJigB.model_ID      = stProductData.model_ID;
                m_ProductDataJigB.model_Name    = stProductData.model_Name;
                m_ProductDataJigB.model_Suffix  = stProductData.model_Suffix;
                m_ProductDataJigB.ORGID         = stProductData.ORGID;
                m_ProductDataJigB.TID           = stProductData.TID;
                m_ProductDataJigB.WorkID        = stProductData.WorkID;
                //m_ProductDataJigB.LsSubItem     = m_ProductData.LsSubItem;
                m_ProductDataJigB.strStatorType = stProductData.strStatorType;
            }
        }

        public void SetOldProductData(int nJigNo, EPDS stProductData)
        {
            if (nJigNo == 0) // Jig A
            {
                m_ProductDataOldJigA.LineID = stProductData.LineID;
                m_ProductDataOldJigA.model_ID = stProductData.model_ID;
                m_ProductDataOldJigA.model_Name = stProductData.model_Name;
                m_ProductDataOldJigA.model_Suffix = stProductData.model_Suffix;
                m_ProductDataOldJigA.ORGID = stProductData.ORGID;
                m_ProductDataOldJigA.TID = stProductData.TID;
                m_ProductDataOldJigA.WorkID = stProductData.WorkID;
                m_ProductDataOldJigA.LsSubItem = stProductData.LsSubItem;
                m_ProductDataOldJigA.strStatorType = stProductData.strStatorType;
            }
            else if (nJigNo == 1) // Jig B
            {
                m_ProductDataOldJigB.LineID = stProductData.LineID;
                m_ProductDataOldJigB.model_ID = stProductData.model_ID;
                m_ProductDataOldJigB.model_Name = stProductData.model_Name;
                m_ProductDataOldJigB.model_Suffix = stProductData.model_Suffix;
                m_ProductDataOldJigB.ORGID = stProductData.ORGID;
                m_ProductDataOldJigB.TID = stProductData.TID;
                m_ProductDataOldJigB.WorkID = stProductData.WorkID;
                m_ProductDataOldJigB.LsSubItem = stProductData.LsSubItem;
                m_ProductDataOldJigB.strStatorType = stProductData.strStatorType;
            }
        }

        public EPDS GetProductData(int nJigNo)
        {
            if (nJigNo == 0) // Jig A
            {
                if(m_ProductDataOldJigA.SetID.Count() > 0)
                {
                    return m_ProductDataOldJigA;
                }
                else
                {
                    return m_ProductDataJigA;
                }
            }
            else if(nJigNo == 1)
            {
                if (m_ProductDataOldJigB.SetID.Count() > 0)
                {
                    return m_ProductDataOldJigB;
                }
                else
                {
                    return m_ProductDataJigB;
                }
            }
            else
            {
                return m_ProductData;
            }
        }

        public bool CheckProductData(int nJigNo)
        {
            // Barcode 정보가 없으면 Error
            if(GetProductData(nJigNo).SetID.Count() == 0)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        public EAYT_R GetEquipInfo()
        {
            return m_EQPInfo;
        }

        // 메세지를 만들어 송신하는 함수 -------------------------------------

        protected void ReadFileAddSendBuffer(string strPath)
        {
            if (IsConnected)
            {
                // 리스트의 첫번째 string Data를 binary(byte) 데이터로 변환
                byte[] TmpData = Encoding.ASCII.GetBytes(m_clsJSON.ReadFile(strPath));

                // Make Message(처음과 끝에 STX, ETX 추가)
                byte[] SndData = new byte[TmpData.Length + 2];
                SndData[0] = 0x02;
                Array.Copy(TmpData, 0, SndData, 1, TmpData.Length);
                SndData[TmpData.Length + 1] = 0x03;

                // Send Data
                Send(SndData);
            }
        }

        public void SendELNT_ACK(string strEqpID)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\ELNT_R.json";
            m_clsJSON.MakeELNT_ACK(strPath, strEqpID);

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bEQP_ELNT_R = true; // Ack ON
            m_bGMES_ELNT = false; // Req Off

            UpdateLinkTestAck();
        }

        public void SendEDTS_ACK(string strEqpID, int nACK)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\EDTS_R.json";
            m_clsJSON.MakeEDTS_ACK(strPath, strEqpID, nACK);

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bEQP_EDTS_R = true; // Ack ON
            m_bGMES_EDTS = false; // Req Off
        }

        public void SendEPDS_ACK(string strEqpID)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\EPDS_R.json";
            m_clsJSON.MakeEPDS_ACK(strPath, m_EQPInfo, m_ProductData);

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bEQP_EDTS_R = true; // Ack ON
            m_bGMES_EDTS = false; // Req Off
        }

        public void SendEAYT()
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\EAYT.json";
            m_clsJSON.MakeEAYT(strPath);

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bGMES_EAYT_R = false; // Respon Off
            m_bEQP_EAYT = true;     // Req On
        }

        public void SendEEMR(string strEqpID, string strEqpMode)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\EEMR.json";
            m_clsJSON.MakeEEMR(strPath, strEqpID, strEqpMode);

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bGMES_EEMR_R = false; // Respon Off
            m_bEQP_EEMR = true; // Req On
        }

        public void SendEERS(string strEqpID, string strCEID, string strRPTID, string strLOTID, int nJigNo)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\EERS.json";
            m_clsJSON.MakeEERS(strPath, strEqpID, strLOTID, strCEID, strRPTID);

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bGMES_EERS_R = false; // Respons Off
            m_bEQP_EERS = true;     // Req On

            // Set Product Data (LOT ID)
            if (nJigNo == 0)
            {
                m_ProductDataJigA.SetID = strLOTID;

                // Set Flag
                m_stStatusJigA.bGMES_EERS_R = false;    // Respons Off
                m_stStatusJigA.bEQP_EERS = true;        // Req On

                // Set EERS_R Data
                m_EERS_Ack.nJigType = eJigType.JigA;

            }
            else
            {
                m_ProductDataJigB.SetID = strLOTID;

                // Set Flag
                m_stStatusJigB.bGMES_EERS_R = false;    // Respons Off
                m_stStatusJigB.bEQP_EERS = true;        // Req On

                // Set EERS_R Data
                m_EERS_Ack.nJigType = eJigType.JigB;
            }
            
        }

        public void SendEIDS(List<InspDataGMES> InsptData, string strModelNo, int nReworkCnt, string strBarCode, string strJigType, EPDS producData, EAYT_R EQPData, string strFinalJudgement, int nJigNo, bool bRepair, ExtInfo extInfo)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\EIDS.json";
            string strJigName = null;
            if(nJigNo == 0)
            {
                strJigName = string.Format("A");
                // Make EIDS Message
                m_clsJSON.MakeEIDS(strPath, InsptData, strModelNo, nReworkCnt, strBarCode, strJigType, m_ProductDataJigA, EQPData, strFinalJudgement, strJigName, bRepair, extInfo);
            }
            else
            {
                strJigName = string.Format("B");
                // Make EIDS Message
                m_clsJSON.MakeEIDS(strPath, InsptData, strModelNo, nReworkCnt, strBarCode, strJigType, m_ProductDataJigB, EQPData, strFinalJudgement, strJigName, bRepair, extInfo);
            }
            

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);

            // Set Flag
            m_bGMES_EIDS_R = false; // Respon Off
            m_bEQP_EIDS = true; // Req On
            if(nJigNo == 0)
            {
                m_stStatusJigA.bGMES_EIDS_R = false;
                m_stStatusJigA.bEQP_EIDS = true;
            }
            else
            {
                m_stStatusJigB.bGMES_EIDS_R = false;
                m_stStatusJigB.bEQP_EIDS = true;
            }
        }

        public JIG_MESSTATUS GetMESStatusJigA()
        {
            return m_stStatusJigA;
        }

        public JIG_MESSTATUS GetMESStatusJigB()
        {
            return m_stStatusJigB;
        }

        public EERS_R GetEERS_R()
        {
            return m_EERS_Ack;
        }

        public EIDS_R GetEIDS_R()
        {
            return m_EIDS_Ack;
        }

        public void SetProductInJig(int nJigNo, bool bInJig)
        {
            if(nJigNo == 0)
            {
                m_stStatusJigA.bProductInJig = bInJig;
            }
            else
            {
                m_stStatusJigB.bProductInJig = bInJig;
            }
        }

        public void SetEIDS_R_Flag(int nJigNo, bool bRcv)
        {
            if(nJigNo == 0)
            {
                if(bRcv)
                {
                    m_stStatusJigA.bEQP_EIDS = false;
                    m_stStatusJigA.bGMES_EIDS_R = true;
                }
                else
                {
                    m_stStatusJigA.bEQP_EIDS = true;
                    m_stStatusJigA.bGMES_EIDS_R = false;
                }
            }
            else
            {
                if (bRcv)
                {
                    m_stStatusJigB.bEQP_EIDS = false;
                    m_stStatusJigB.bGMES_EIDS_R = true;
                }
                else
                {
                    m_stStatusJigB.bEQP_EIDS = true;
                    m_stStatusJigB.bGMES_EIDS_R = false;
                }
            }
        }

        public void SetEPDS_Flag(int nJigNo, bool bRcv)
        {
            if (nJigNo == 0)
            {
                if (bRcv)
                {
                    m_stStatusJigA.bEQP_EPDS_R = false;
                    m_stStatusJigA.bGMES_EPDS = true;
                }
                else
                {
                    m_stStatusJigA.bEQP_EPDS_R = true;
                    m_stStatusJigA.bGMES_EPDS = false;
                }
            }
            else
            {
                if (bRcv)
                {
                    m_stStatusJigB.bEQP_EPDS_R = false;
                    m_stStatusJigB.bGMES_EPDS = true;
                }
                else
                {
                    m_stStatusJigB.bEQP_EPDS_R = true;
                    m_stStatusJigB.bGMES_EPDS = false;
                }
            }
        }

        public void SendESRR(int nStopType, int nSetReset, int nStopReason, int nJigNo)
        {
            // Make Json File
            string strPath = Application.StartupPath + "\\ESRR.json";
            if (nJigNo == 0)
            {
                m_clsJSON.MakeESRR(strPath, m_EQPInfo, m_ProductDataJigA, nStopType, nSetReset, nStopReason);
            }
            else
            {
                m_clsJSON.MakeESRR(strPath, m_EQPInfo, m_ProductDataJigB, nStopType, nSetReset, nStopReason);
            }

            // Add to Send Buffer
            ReadFileAddSendBuffer(strPath);
        }
    }
}
