using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

using HubisCommonControl;  // 휴비스 공통 클래스 사용하기 위해 선언(Log, Socket 등)

namespace MIS.Common
{
    public class JSONData
    {
        //부모 노드 이름
        public string ParaentNode = string.Empty;

        //노드 이름
        public string NodeName = string.Empty;

        //값
        public string Value = string.Empty;
    }

    public class JSONReader
    {
        //XML 객체

        public static readonly string[] m_strRcvNode = {
            "ELNT",
            "EAYT_R",
            "LDTR",
            "LDTR_R",
            "EEMR_R",
            "EESR_R",
            "EDTS",
            "EDTR",
            "EASR_R",
            "EARR_R",
            "EERS_R",
            "EAPD_R",
            "EPDS",
            "EIDS_R",
            "ETPD_R",
            "EORR_R",
            "ERPS",
            "ERCP_R",
            "ERLD",
            "ERPR",
            "MCSC_R",
            "TDCR_R",
            "MFUJ",
            "MFTT",
            "ESRR_R"
        };

        public enum RECEVIE_NODE
        {
            ELNT,
            EAYT_R,
            LDTR,
            LDTR_R,
            EEMR_R,
            EESR_R,
            EDTS,
            EDTR,
            EASR_R,
            EARR_R,
            EERS_R,
            EAPD_R,
            EPDS,
            EIDS_R,
            ETPD_R,
            EORR_R,
            ERPS,
            ERCP_R,
            ERLD,
            ERPR,
            MCSC_R,
            TDCR_R,
            MFUJ,
            MFTT,
            ESRR_R
        }

        //디버깅용 Send, Recv Data 파일 저장 경로        
        //public string sendFilePath = Path.Combine(Application.StartupPath, @"SendData.xml");
        //public string recvFilePath = Path.Combine(Application.StartupPath, @"RecvData.xml");

        public string sendeQPIDResqFile = Path.Combine(Application.StartupPath, @"sendEAYT.json");
        public string recveQPIDFile = Path.Combine(Application.StartupPath, @"recvEAYT_R.json");

        public string sendlinkRespFile = Path.Combine(Application.StartupPath, @"sendELNT_R.json");
        public string recvlinkReqFile = Path.Combine(Application.StartupPath, @"recvELNT.json");

        public string sendDateTImeACKFile = Path.Combine(Application.StartupPath, @"sendEDTR_R.json");
        public string recvDateTimeFile = Path.Combine(Application.StartupPath, @"recvEDTR.json");

        public string sendInsptDataFile = Path.Combine(Application.StartupPath, @"sendEIDS.json");
        public string recvInsptDataACKFile = Path.Combine(Application.StartupPath, @"recvEIDS_R.json");

        public string weldParamPath = Path.Combine(Application.StartupPath, @"Offline.csv");

        public string sendProcDataACKFile = Path.Combine(Application.StartupPath, @"sendEPDS_R.json");
        public string recvProcDataFile = Path.Combine(Application.StartupPath, @"recvEPDS.json");

        public string sendEventRpFile = Path.Combine(Application.StartupPath, @"sendEERS.json");
        public string recvEventRpFile = Path.Combine(Application.StartupPath, @"recvEERS_R.json");

        public string sendEEMRFile = Path.Combine(Application.StartupPath, @"sendEEMR.json");

        public string ErrorFile = Path.Combine(Application.StartupPath, @"Error.json");

        public string sendESRRFile = Path.Combine(Application.StartupPath, @"sendESRR.json");
        public string recvESRR_RFile = Path.Combine(Application.StartupPath, @"recvESRR_R.json");

        //수신 받은 Json Data 저장하는 객체
        private List<JSONData> jSONDatas = new List<JSONData>();
        public List<JSONData> GetJSONDatas()
        {
            return jSONDatas;
        }

        /// <summary>
        /// string 형태로 파일 읽는 함수
        /// </summary>
        /// <param name="FilePath"></param>
        /// <returns></returns>
        public string ReadFile(string FilePath)
        {
            try
            {
                using (FileStream fs = new FileStream(FilePath, FileMode.Open, FileAccess.Read))
                {
                    StreamReader sw = new StreamReader(fs);
                    string ReadData = sw.ReadToEnd();

                    sw.Close();
                    fs.Close();

                    return ReadData;
                }
            }
            catch (FileNotFoundException ex)
            {
                LogManager.WriteLog(LogType.Error, ex.Message);
                return null;
            }
        }

        /// <summary>
        /// 수신된 데이터 string Data File로 쓰는 함수
        /// </summary>
        /// <param name="FilePath"></param>
        /// <param name="Data"></param>        
        public void WriteFile(string strData, string strPath)
        {
            try
            {
                using (FileStream fs = new FileStream(strPath, FileMode.Create, FileAccess.Write))
                {
                    byte[] recvData = Encoding.ASCII.GetBytes(strData);

                    fs.Write(recvData, 0, recvData.Length);
                    fs.Close();
                    //LogManager.WriteLog(LogType.Information, "The received file has been saved. Saved file name = " + strPath);
                }
            }
            catch (FileNotFoundException ex)
            {
                LogManager.WriteLog(LogType.Error, ex.Message);
            }
        }


        /// <summary>
        /// JSON 형태로 읽는 함수
        /// 파일을 읽어, Node와 Data로 분리하여 저장.
        /// </summary>
        /// <param name="FilePath"></param>        
        public void ReadJsonFile(string FilePath)
        {
            try
            {
                StreamReader file = File.OpenText(FilePath);
                JsonTextReader reader = new JsonTextReader(file);

                JObject jobjRoot = (JObject)JToken.ReadFrom(reader);

                int nCheckNode = 0;

                // Check JSON Node
                string strRoot = string.Empty;
                nCheckNode = CheckJsonNode(jobjRoot, ref strRoot);

                // Parsing JSON Data
                if (nCheckNode < 0)
                {
                    LogManager.WriteLog(LogType.Error, "!!! This is not Supported Message ID !!!");

                    reader.Close();
                    file.Close();

                    return;
                }
                if(ParseJsonData(jobjRoot, nCheckNode) == false)
                {
                    LogManager.WriteLog(LogType.Error, "!!! JSON Paring Error !!!");

                    reader.Close();
                    file.Close();

                    return;
                }

            }
            catch (FileNotFoundException ex)
            {
                LogManager.WriteLog(LogType.Error, ex.Message);
            }
        }

        public string ReadJsonFileAndRename(string FilePath)
        {
            string strRootName = "";
            try
            {
                // 변수 선언
                StreamReader file = File.OpenText(FilePath);
                JsonTextReader reader = new JsonTextReader(file);

                // Read File in JSON Format
                JObject jobjRoot = (JObject)JToken.ReadFrom(reader);

                // 어떤 메세지인지 Msg ID(Node) 확인
                // nCheckNo : int형 Message ID
                // strRootName : string형 Message ID
                int nCheckNode = 0;
                nCheckNode = CheckJsonNode(jobjRoot, ref strRootName);

                // Error - Node를 못찾으면, 미리 정의된 Message ID에 포함되지 않음.
                if (nCheckNode < 0)
                {
                    LogManager.WriteLog(LogType.Error, "!!! This is not Supported Message ID !!!");

                    reader.Close();
                    file.Close();

                    strRootName = "";

                    return strRootName;
                }

                // Parsing JSON Data
                if (ParseJsonData(jobjRoot, nCheckNode) == false)
                {
                    // Parsing Error
                    LogManager.WriteLog(LogType.Error, "!!! JSON Paring Error !!!");

                    reader.Close();
                    file.Close();

                    strRootName = "";

                    return strRootName;
                }
         

                // File Close 하고, Root Name(Message ID 명)으로 파일명 변경
                file.Close();

                string strNewName = string.Empty;
                if (strRootName.Length > 0)
                {
                    strNewName = Application.StartupPath + "\\recv" + strRootName + ".json";
                    System.IO.File.Copy(FilePath, strNewName, true);
                }

                System.IO.File.Delete(FilePath);

                return strRootName;
            }
            catch (FileNotFoundException ex)
            {
                LogManager.WriteLog(LogType.Error, ex.Message);

                return strRootName;
            }
        }

        public int CheckJsonNode(JObject jobjRoot, ref string rootName)
        {
            int nMsgIndex = -1;

            for (int i = 0; i < m_strRcvNode.Length; i++)
            {
                var node = jobjRoot.SelectToken(m_strRcvNode[i].ToString());
                if(node != null)
                {
                    nMsgIndex = i;
                    rootName = m_strRcvNode[i];
                    break;
                }
                
            }

            return nMsgIndex;
        }

        public bool ParseJsonData(JObject jobRoot, int nRcvType)
        {
            bool bRtn = false;
            // Parsing
            switch ((RECEVIE_NODE)nRcvType)
            {
                case RECEVIE_NODE.ELNT:     // Link Test
                    bRtn = ParseLinkTest(jobRoot);
                    break;
                case RECEVIE_NODE.EAYT_R:   // Are You There Request Acknowledge
                    bRtn = ParseAreYouThererAck(jobRoot);
                    break;
                case RECEVIE_NODE.LDTR:  
                    break;
                case RECEVIE_NODE.LDTR_R:
                    break;
                case RECEVIE_NODE.EEMR_R: // Equipment Mode Report(Online / Offline) Ack
                    bRtn = ParseEquipmentModeReportAck(jobRoot);
                    break;
                case RECEVIE_NODE.EESR_R: // Equipment State Report(Idle / run) Ack
                    bRtn = ParseEquipmentStateReportAck(jobRoot);
                    break;
                case RECEVIE_NODE.EDTS: // Equipment Date and Time Send
                    bRtn = ParseDateTimeSend(jobRoot);
                    break;
                case RECEVIE_NODE.EDTR:
                    break;
                case RECEVIE_NODE.EASR_R:
                    break;
                case RECEVIE_NODE.EARR_R:
                    break;
                case RECEVIE_NODE.EERS_R: // Event Report Send Acknowledge
                    bRtn = ParseEventReportSendAck(jobRoot);
                    break;
                case RECEVIE_NODE.EAPD_R: // Actual Process Data Send
                    bRtn = ParseActuaProcessDataSendAck(jobRoot);
                    break;
                case RECEVIE_NODE.EPDS: // Product Data Send
                    bRtn = ParseProductDataSend(jobRoot);
                    break;
                case RECEVIE_NODE.EIDS_R: // Equipment Inspection(Processing) Data Send Ack
                    bRtn = ParseInspectionDataSendAck(jobRoot);
                    break;
                case RECEVIE_NODE.ETPD_R:
                    break;
                case RECEVIE_NODE.EORR_R:
                    break;
                case RECEVIE_NODE.ERPS:
                    break;
                case RECEVIE_NODE.ERCP_R:
                    break;
                case RECEVIE_NODE.ERLD:
                    break;
                case RECEVIE_NODE.ERPR:
                    break;
                case RECEVIE_NODE.MCSC_R:
                    break;
                case RECEVIE_NODE.TDCR_R:
                    break;
                case RECEVIE_NODE.MFUJ:
                    break;
                case RECEVIE_NODE.MFTT:
                    break;
                case RECEVIE_NODE.ESRR_R: // Equipment Stop Reason Report Ack
                    bRtn = ParseEquipStopReasonReportAck(jobRoot);
                    break;
                default:
                    bRtn = false;
                    break;
            }

            return bRtn;
        }

        public void AddReceivedData(string strParentNodeName, string strNodeName, string strValue)
        {
            JSONData data = new JSONData();

            data.ParaentNode = strParentNodeName;
            data.NodeName = strNodeName;
            data.Value = strValue;

            jSONDatas.Add(data);
        }

        public bool GetJsonValue(JToken jtokMain, string strNodeName, ref string strValue, string strSubNodeName = "")
        {
            if (jtokMain[strNodeName] == null)
            {
                return false;
            }
            else
            {
                if (strSubNodeName.Length > 0)
                {
                    if(jtokMain[strNodeName][strSubNodeName] == null)
                    {
                        return false;
                    }
                    else
                    {
                        strValue = jtokMain[strNodeName][strSubNodeName].ToString();
                    }
                }
                else
                {
                    strValue = jtokMain[strNodeName].ToString();
                }
                return true;
            }
        }

        public int GetJsonValueCnt(JToken jtokMain, string strNodeName)
        {
            if (jtokMain[strNodeName] == null)
            {
                return 0;
            }
            else
            {
                return jtokMain[strNodeName].Count();
            }
        }

        public bool GetJsonSubValue(JToken jtokMain, string strNodeName, string strSubNodeName, int nIndex, ref string strValue)
        {
            if (jtokMain[strNodeName][nIndex][strSubNodeName] == null)
            {
                return false;
            }
            else
            {
                strValue = jtokMain[strNodeName][nIndex][strSubNodeName].ToString();
                return true;
            }
        }


        public bool ParseAreYouThererAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EAYT_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // EQPNAME
            NodeName = "EQPNAME";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // PROCID
            NodeName = "PROCID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // LINEID
            NodeName = "LINEID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ORGID
            NodeName = "ORGID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;

        }

        public bool ParseInspectionDataSendAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EIDS_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // TID
            NodeName = "TID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // WOID
            NodeName = "WOID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ORGID
            NodeName = "ORGID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // LINEID
            NodeName = "LINEID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // PROCID
            NodeName = "PROCID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // SETID
            NodeName = "SETID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // MODEL -> ID
            NodeName = "MODEL";
            string SubNodeName = "ID";
            if (GetJsonValue(jTokMain, NodeName, ref Value, SubNodeName) == false)
            {
                return false;
            }
            AddReceivedData(NodeName, SubNodeName, Value);

            // MODEL -> NAME
            SubNodeName = "NAME";
            if (GetJsonValue(jTokMain, NodeName, ref Value, SubNodeName) == false)
            {
                return false;
            }
            AddReceivedData(NodeName, SubNodeName, Value);

            // MODEL -> SUFFIX
            SubNodeName = "SUFFIX";
            if (GetJsonValue(jTokMain, NodeName, ref Value, SubNodeName) == false)
            {
                return false;
            }
            AddReceivedData(NodeName, SubNodeName, Value);

            // ACK
            NodeName = "ACK";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // REASON
            NodeName = "REASON";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }

        public bool ParseActuaProcessDataSendAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EAPD_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // ORGID
            NodeName = "ORGID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // LINEID
            NodeName = "LINEID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // PROCID
            NodeName = "PROCID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // SETID
            NodeName = "SETID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ACK
            NodeName = "ACK";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // REASON
            NodeName = "REASON";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }

        public bool ParseEquipmentStateReportAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EESR_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ACK
            NodeName = "ACK";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }

        public bool ParseEquipmentModeReportAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EEMR_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ACK
            NodeName = "ACK";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }

        public bool ParseLinkTest(JObject jobRoot)
        {
            bool bRtn = false;

            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가
            JToken jTokMain = jobRoot["ELNT"];

            ParentNodeName = jTokMain.Path.ToString();
            NodeName = "";
            Value = "";
            AddReceivedData(ParentNodeName, NodeName, Value);

            bRtn = true;

            return bRtn;
        }


        public bool ParseDateTimeSend(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EDTS"];
            ParentNodeName = jTokMain.Path.ToString();

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ACK
            NodeName = "TIME";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }

        public bool ParseEventReportSendAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EERS_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // CEID
            NodeName = "CEID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // RPTID
            NodeName = "RPTID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ACK
            NodeName = "ACK";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // REASON
            NodeName = "REASON";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }
        
        public bool ParseProductDataSend(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string SubNodName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["EPDS"];
            ParentNodeName = jTokMain.Path.ToString();

            // TID
            NodeName = "TID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // WOID
            NodeName = "WOID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ORGID
            NodeName = "ORGID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // LINEID
            NodeName = "LINEID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // PROCID
            NodeName = "PROCID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // SETID
            NodeName = "SETID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // Model->ID
            NodeName = "MODEL";
            SubNodName = "ID";
            if (GetJsonValue(jTokMain, NodeName, ref Value, SubNodName) == false)
            {
                return false;
            }
            AddReceivedData(NodeName, SubNodName, Value);

            // Model->NAME
            NodeName = "MODEL";
            SubNodName = "NAME";
            if (GetJsonValue(jTokMain, NodeName, ref Value, SubNodName) == false)
            {
                return false;
            }
            AddReceivedData(NodeName, SubNodName, Value);

            // Model->SUFFIX
            NodeName = "MODEL";
            SubNodName = "SUFFIX";
            if (GetJsonValue(jTokMain, NodeName, ref Value, SubNodName) == false)
            {
                return false;
            }
            AddReceivedData(NodeName, SubNodName, Value);

            // SubItem
            NodeName = "SUBITEM_LIST";
            int nSubItemCnt = GetJsonValueCnt(jTokMain, NodeName);
            for(int i = 0; i< nSubItemCnt; i++)
            {
                // NAME
                SubNodName = string.Format("NAME");
                if(GetJsonSubValue(jTokMain, NodeName, SubNodName, i, ref Value) == false)
                {
                    return false;
                }
                AddReceivedData(NodeName, SubNodName, Value);

                // VALUE
                SubNodName = string.Format("VALUE");
                if (GetJsonSubValue(jTokMain, NodeName, SubNodName, i, ref Value) == false)
                {
                    return false;
                }
                AddReceivedData(NodeName, SubNodName, Value);
            }

            return true;
        }

        public bool ParseEquipStopReasonReportAck(JObject jobRoot)
        {
            // 수신데이터 클리어
            jSONDatas.Clear();

            // 수신데이터 변수 선언
            string ParentNodeName = string.Empty;
            string NodeName = string.Empty;
            string Value = string.Empty;

            // 파싱하여 수신데이터 리스트(jSONDatas)에 추가 시작 -----
            JToken jTokMain = jobRoot["ESRR_R"];
            ParentNodeName = jTokMain.Path.ToString();

            // EQPID
            NodeName = "EQPID";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // ACK
            NodeName = "ACK";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            // REASON
            NodeName = "REASON";
            if (GetJsonValue(jTokMain, NodeName, ref Value) == false)
            {
                return false;
            }
            AddReceivedData(ParentNodeName, NodeName, Value);

            return true;
        }


        /// <summary>
        /// EIF ID 판별(즉, 정상적으로 데이터가 수신됬는지 판단)
        /// </summary>
        /// <param name="Element"></param>
        /// <param name="GroupID"></param>
        public string GetEIFGroup(JObject jObj)
        {
            //<EIF VERSION="1.9" ID="S6F1" NAME="Inspection (Processing) Data Send">

            string GroupID = string.Empty;

            GroupID = (string)jObj["ID"].ToString();

            return GroupID;
        }

        #region PC -> GMES (XML File Write 하는 함수들)
        /// <summary>
        /// Are You There Request(EQPID = Equipment ID 가져오기 위한 GMES 요청 신호) PC -> GMES
        /// </summary>
        /// <param name="FilePath">파일 경로</param>
        public void MakeEAYT(string FilePath)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            JObject jobFile = new JObject(
                new JProperty("EAYT", jobRoot)
                );

            File.WriteAllText(FilePath, jobFile.ToString());
        }

        /// <summary>
        /// PC -> GMES(EEMR)
        /// EquipmentModeReport 파일 작성
        /// </summary>
        /// <param name="FilePath"></param>
        public void MakeEEMR(string FilePath, string strEQPID, string strMode)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            // EQPID
            strNode = "EQPID";
            strValue = strEQPID;
            jobRoot.Add(strNode, strValue);

            strNode = "CONTROLSTATE";
            strValue = strMode;
            jobRoot.Add(strNode, strValue);

            JObject jobFile = new JObject(
                new JProperty("EEMR", jobRoot)
                );

            File.WriteAllText(FilePath, jobFile.ToString());
        }

        /// <summary>
        /// PC -> GMES(ELNT_R)
        /// Link Test Response(PC와 GMES Link Test를 하기 위한 응답) 데이터 파일로 작성 
        /// </summary>
        /// <param name="FilePath"></param>
        public void MakeELNT_ACK(string FilePath, string strEQPID)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            // EQPID
            strNode = "EQPID";
            strValue = strEQPID;
            jobRoot.Add(strNode, strValue);

            JObject jobFile = new JObject(
                new JProperty("ELNT_R", jobRoot)
                );

            File.WriteAllText(FilePath, jobFile.ToString());
        }

        /// <summary>
        /// PC -> GMES(EDTS_R)
        /// Date and Time Acknowledge(수신 받은 Date Time에 대한 응답) 파일 생성.
        /// </summary>
        /// <param name="FilePath"></param>
        /// <param name="strEQPID">EQPID</param>
        /// <param name="nACK"> 0 = Accepted, 1 = Not Accepted</param>
        public void MakeEDTS_ACK(string FilePath, string strEQPID, int nACK)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            // EQPID
            strNode = "EQPID";
            strValue = strEQPID;
            jobRoot.Add(strNode, strValue);

            // ACK
            strNode = "ACK";
            strValue = nACK.ToString();
            jobRoot.Add(strNode, strValue);

            JObject jobFile = new JObject(
                new JProperty("EDTS_R", jobRoot)
                );

            File.WriteAllText(FilePath, jobFile.ToString());
        }

        /// <summary>
        /// PC -> GMES(EIDS)
        /// Equipment Product Data Send (1Stator에 대한 데이터 전송) PC -> GMES
        /// </summary>
        /// <param name="FilePath">저장할 파일 경로</param>
        /// <param name="InsptData">데이터 담고 있는 리스트</param>
        public void MakeEIDS(string FilePath, List<InspDataGMES> InsptData, string strModelNum, int nReworkCnt, string strBarCode, string strJigType, EPDS producData, EAYT_R EQPData, string strFinalJudgement, string strJigName, bool bRepair, ExtInfo extInfo)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            jobRoot.Add("TID", producData.TID);
            jobRoot.Add("WOID", producData.WorkID);
            jobRoot.Add("ORGID", EQPData.ORGID);
            jobRoot.Add("LINEID", EQPData.LineID);
            jobRoot.Add("EQPID", EQPData.EQPID);
            jobRoot.Add("PROCID", EQPData.ProcID);
            jobRoot.Add("SETID", strBarCode);

            // Model
            JObject jobNode = new JObject();
            jobNode.Add("ID", producData.model_ID);
            jobNode.Add("NAME", producData.model_Name);
            jobNode.Add("SUFFIX", producData.model_Suffix);

            jobRoot.Add("MODEL", jobNode);

            // Result
            jobRoot.Add("RESULT", strFinalJudgement);

            // Subitem List
            JObject jobNodeSubitem = new JObject();

            JArray jArrayMain = new JArray();

            if (bRepair == false)
            {
                // WELD_EQPID
                JObject jobSubEqp = new JObject();
                jobSubEqp.Add("NAME", "WELD_EQPID");
                jobSubEqp.Add("VALUE", EQPData.EQPID);
                jArrayMain.Add(jobSubEqp);

                // JIG NAME
                JObject jobSubJig = new JObject();
                jobSubJig.Add("NAME", "STATION_NAME");
                jobSubJig.Add("VALUE", strJigName);
                jArrayMain.Add(jobSubJig);
            }

            // Ext Info
            JObject jobSubLoadRate = new JObject();
            jobSubLoadRate.Add("NAME", "PEAK_LOAD_RATE");
            string strLoadRate = string.Format("HEAD_X,{0},HEAD_Z,{1},CLAMP_Y,{2},CLAMP_Z,{3},CLAMP_C,{4},CENTERING_Z,{5}",
                                                extInfo.LoadRate_HeadX,
                                                extInfo.LoadRate_HeadZ,
                                                extInfo.LoadRate_ClampY,
                                                extInfo.LoadRate_ClampZ,
                                                extInfo.LoadRate_ClampC,
                                                extInfo.LoadRate_CenteringZ);
            jobSubLoadRate.Add("VALUE", strLoadRate);
            jArrayMain.Add(jobSubLoadRate);


            string strResult = "OK";
            bool bRwData = false;
            int nRWCnt = 0;
            for (int i= 0; i<InsptData.Count(); i++)
            {
                JObject jobSub = new JObject();
                // Name
                string Value = InsptData[i].Slot + "_" + InsptData[i].Layer;
                jobSub.Add("NAME", Value);

                // Value
                // sjyi 2024.02.25 각 아이템별 콤마(,) 추가
                Value = "1H," + InsptData[i].H1 + ",2H," + InsptData[i].H2 + ",C," + InsptData[i].CValue + ",J," + InsptData[i].Jump + ",T," + InsptData[i].Theta;
                Value += ",1H," + InsptData[i].Judge_H1 + ",2H," + InsptData[i].Judge_H2 + ",C," + InsptData[i].Judge_CValue + ",J," + InsptData[i].Judge_Jump + ",T," + InsptData[i].Judge_Theta +
                         ",FJ," + strFinalJudgement;

                for (int j = 0; j < InsptData[i].LsJudgement.Count(); j++)
                {
                    int nTmp = j + 1;
                    string strTmp = ",D" + nTmp + ",";

                    if(InsptData[i].LsJudgement[j] == "OK" || InsptData[i].LsJudgement[j] == "NG")
                    {
                        Value += strTmp + InsptData[i].LsJudgement[j];
                        bRwData = false;
                    }
                    else
                    {
                        bRwData = true;
                        if (InsptData[i].LsJudgement[j] == "RW_OK")
                        {
                            Value += strTmp + "OK";
                        }
                        else
                        {
                            Value += strTmp + "NG";
                        }
                    }

                    if (j == InsptData[i].LsJudgement.Count() - 1)
                    {
                        if (InsptData[i].LsJudgement[j] == "NG")
                        {
                            strResult = "NG";
                        }
                    }
                }

                if (bRwData == true)
                {
                    Value += ",RW";
                    nRWCnt++;
                }

                jobSub.Add("VALUE", Value);

                // Array에 추가
                jArrayMain.Add(jobSub);
            }

            if(bRepair)
            {
                // Final Result
                JObject jobSubRWCnt = new JObject();
                jobSubRWCnt.Add("NAME", "RW_CNT");
                jobSubRWCnt.Add("VALUE", nRWCnt.ToString());
                jArrayMain.Add(jobSubRWCnt);
            }
            else
            {
                // Final Result
                JObject jobSubResult = new JObject();
                jobSubResult.Add("NAME", "RESULT");
                jobSubResult.Add("VALUE", strResult);
                jArrayMain.Add(jobSubResult);
            }

            // Add SubItem
            jobRoot.Add("SUBITEM_LIST",jArrayMain);

            JObject jobFile = new JObject(
                new JProperty("EIDS", jobRoot)
                );

            File.WriteAllText(FilePath, jobFile.ToString());

        }

        /// <summary>
        /// PC -> GMES(EERS)
        ///  Event Report Send 데이터 파일로 저장
        /// </summary>
        /// <param name="filePath">파일 저장할 경로</param>
        /// <param name="EventRpData">LG에서 정해준 EventReport 데이터</param>        
        /// <param name="strBarcode">PLC로부터 수신받은 Barcode(LotID)</param>
        /// <param name="strCEID">LGE 측에서 Define 해준 CEID(Config에서 수정 가능)</param>
        /// <param name="strRPTID">LGE 측에서 Define 해준 CEID(Config에서 수정 가능)</param>
        public void MakeEERS(string filePath, string strEQPID, string strBarcode, string strCEID, string strRPTID)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            jobRoot.Add("EQPID", strEQPID);
            jobRoot.Add("CEID", strCEID);
            jobRoot.Add("RPTID", strRPTID);


            // Subitem List
            JObject jobNodeSubitem = new JObject();
            JArray jArrayMain = new JArray();
            //for (int i = 0; i < 1; i++)
            //{
            JObject jobSub = new JObject();
            // Name
            jobSub.Add("NAME", "SETID");
            // Value(SETID, Barcode)
            jobSub.Add("VALUE", strBarcode);
            // Array에 추가
            jArrayMain.Add(jobSub);

            jobSub = new JObject();
            // Name
            jobSub.Add("NAME", "SPEC_DOWNLOAD_YN");
            // Value(SETID, Barcode)
            jobSub.Add("VALUE", "Y");
            // Array에 추가
            jArrayMain.Add(jobSub);


            //}

            jobRoot.Add("SUBITEM_LIST", jArrayMain);

            JObject jobFile = new JObject(
                 new JProperty("EERS", jobRoot)
                 );

            File.WriteAllText(filePath, jobFile.ToString());
        }

        /// <summary>
        /// PC -> GMES(EPDS_R)
        /// Product Data Send Acknowledge 
        /// </summary>
        /// <param name="filePath">파일 저장할 경로</param>
        /// <param name="GetProducData">GMES로부터 수신 받은 ProductData 정보</param>        
        public void MakeEPDS_ACK(string filePath, EAYT_R EQPIDData, EPDS productData)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            jobRoot.Add("TID", productData.TID);
            jobRoot.Add("WOID", productData.WorkID);
            jobRoot.Add("ORGID", productData.ORGID);
            jobRoot.Add("LINEID", productData.LineID);
            jobRoot.Add("EQPID", EQPIDData.EQPID);
            jobRoot.Add("PROCID", EQPIDData.ProcID);
            jobRoot.Add("SETID", productData.SetID);

            // Model
            JObject jobNode = new JObject();
            jobNode.Add("ID", productData.model_ID);
            jobNode.Add("NAME", productData.model_Name);
            jobNode.Add("SUFFIX", productData.model_Suffix);

            jobRoot.Add("MODEL", jobNode);

            jobRoot.Add("ACK", "0");
            jobRoot.Add("REASON", "");

            JObject jobFile = new JObject(
                 new JProperty("EPDS_R", jobRoot)
                 );

            File.WriteAllText(filePath, jobFile.ToString());
        }


        /// <summary>
        /// PC -> GMES(ESRR)
        ///  Equipment Stop Reason Report 데이터 파일로 저장
        /// </summary>
        /// <param name="filePath">파일 저장할 경로</param>
        /// <param name="EQPIDData">MES로 받은 EQUIP Data</param>        
        /// <param name="productData">MES로 받은 Product Data</param>
        /// <param name="nSTOPTYPE">정지 타입(1 = Auto / 2 = Manual)</param>
        /// <param name="nSTOPSET">정지 해제(1 = Set / Other = 해제)</param>
        /// <param name="nSTOPREASON">정지 사유(1 = 내/외부 교육 및 훈련 ~ 99 = 사유 없음.)</param>
        public void MakeESRR(string filePath, EAYT_R EQPIDData, EPDS productData, int nSTOPTYPE, int nSTOPSET, int nSTOPREASON)
        {
            JObject jobRoot = new JObject();

            string strNode = string.Empty;
            string strValue = string.Empty;

            jobRoot.Add("EQPID", EQPIDData.EQPID);

            // Model
            JObject jobNode = new JObject();
            jobNode.Add("MODELID", productData.model_ID);
            jobNode.Add("MODELNAME", productData.model_Name);
            jobNode.Add("MODELSUFFIX", productData.model_Suffix);

            jobRoot.Add("MODEL", jobNode);

            // STOPTYPE
            if(nSTOPTYPE == 1)
            {
                jobRoot.Add("STOPTYPE", "AUTO");
            }
            else
            {
                jobRoot.Add("STOPTYPE", "MANUAL");
            }

            // STOPSET
            if (nSTOPSET == 1)
            {
                jobRoot.Add("STOPSET", "SET");
            }
            else
            {
                jobRoot.Add("STOPSET", "RESET");
            }

            switch(nSTOPREASON)
            {
                case 1: // 내/외부 교육 및 훈련
                    jobRoot.Add("STOPREASON", "ADAA");
                    break;
                case 2: // 모델변경
                    jobRoot.Add("STOPREASON", "BBAC");
                    break;
                case 3: // 자재품절
                    jobRoot.Add("STOPREASON", "AAAA");
                    break;
                case 4: // 제품 품질
                    jobRoot.Add("STOPREASON", "BAAA");
                    break;
                case 5: // 예방 정비
                    jobRoot.Add("STOPREASON", "BDAB");
                    break;
                case 6: // 정전
                    jobRoot.Add("STOPREASON", "AGAB");
                    break;
                case 7: // MES Error(통신 끊김)
                    jobRoot.Add("STOPREASON", "BCAB");
                    break;
                case 8: // 설비 준비
                    jobRoot.Add("STOPREASON", "ABAB");
                    break;
                case 9: // 재고 재작업
                    jobRoot.Add("STOPREASON", "DFAA");
                    break;
                case 10: // 설비 고장
                    jobRoot.Add("STOPREASON", "BDAC");
                    break;
                case 11: // 자재 교환
                    jobRoot.Add("STOPREASON", "BBAB");
                    break;
                case 12: // 품질(기타)
                    jobRoot.Add("STOPREASON", "BAAC");
                    break;
                case 13: // 공정(운영)
                    jobRoot.Add("STOPREASON", "BBAD");
                    break;
                default: // 기타
                    jobRoot.Add("STOPREASON", "");
                    break;
            }

            JObject jobFile = new JObject(
                 new JProperty("ESRR", jobRoot)
                 );

            File.WriteAllText(filePath, jobFile.ToString());
        }

        #endregion
    }
}
