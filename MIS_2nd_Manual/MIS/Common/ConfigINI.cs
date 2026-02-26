using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
//INI 필요 lib
using System.Runtime.InteropServices;


namespace MIS.Common
{
    public class ConfigINI
    {
        #region INI kernel
        //Setction 명, Key 명, 값, 파일 위치
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string Section, string Key, string Val, string FilePath);

        //Section 명, Key 명, 기본 값, String Pointer, 크기, 파일 위치
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string Section, string Key, string def, StringBuilder retVal,
                                                        int Size, string FilePath);
        #endregion

        #region GMES 데이터
        public string GMES_IP { get; set; }

        //1개 이상 Driver 일 경우 7000, 7001, 7002 순서대로
        public int GMES_Port { get; set; }

        //회신 시간 범위 1 ~ 120 sec(GMES) default : 45 sec
        public int Timeout { get; set; }

        //Transaction Timeout(Communication Failure이 발생되기 까지 Open된 상태로 있을 수 있는 최대 Time)
        //범위 1 ~ 240 sec(GMES) default : 10 sec
        public int LinkTimeout { get; set; }

        public string strCEID { get; set; }

        public string strRPTID { get; set; }

        #endregion

        #region PLC Info
        //PLC IP
        public string PLCIP { get; set; }

        //PLC Port
        public int PLC_Port { get; set; }

        //PLC Status 영역 시작 위치
        //PLC -> PC
        //D30600 ~ D30601
        public string PLC_ReadStatus { get; set; }

        public string PLC_WriteStatus { get; set; }

        //전 검사 결과(0 = 최초 점 검사 Read 요청(배출보고 후 0으로 초기화), 1 = OK, 2 = NG) - 배출 보고 후 PLC에서 초기화 해줌      
        //PLC에서 읽어올 전 검사 Address 시작 위치        
        public string PLC_ReadBefInspData_A { get; set; }
        public string PLC_ReadBefInspData_B { get; set; }

        //PLC에서 후 검사 결과 Address 시작 위치
        public string PLC_ReadAftInspData_A { get; set; }
        public string PLC_ReadAftInspData_B { get; set; }

        //PLC Address 영역
        public string PLC_Area { get; set; }
        #endregion

        //INI 파일 경로
        public string InIPath { get; set; }

        //HairPin 정보
        public int Slot { get; set; }
        public int Layer { get; set; }

        public int Slot77 { get; set; }
        public int Layer77 { get; set; }

        public int Slot76 { get; set; }
        public int Layer76 { get; set; }

        //생산 로그 기록을 위해 IWS 실행 경로 폴더 저장하는 변수
        public string ProductLogPath { get; set; }

        //0 = None, 1 = 76, 2 = , 3 = , 4 = 
        //public int Model { get; set; }

        public bool m_bUsePLCInterface { get; set; }


        // 2024.01.27 jh.kim 추가 - PLC read시 Retry 간격 및 횟수
        public int m_nRetryCount { get; set; }
        public int m_nRetryDelay { get; set; }

        public bool m_bUseGMESInterface { get; set; }

        private EPDS m_stProductInfoJigA = new EPDS();
        private EPDS m_stProductInfoJigB = new EPDS();

        // sjyi 2023.10.18 2차용접기 여부 확인 플래그 추가
        public bool m_bRepairWeld { get; set; }

        // sjyi 2024.01.15 EQUIP ID List 추가(1차 용접기 검사 결과 확인을 위해 추가)
        int m_nRotateEquipCnt;                      // 검사결과 회전하는 장비 개수
        public List<string> m_listRotateEquipID;    // 검사결과 회전하는 장비 ID 리스트

        //INI Read 하는 함수
        public bool ReadConfig()
        {
            bool nRtn = false;

            GMES_IP = GetINIString("GMES_INFO", "GMES_IP", "127.0.0.1");
            GMES_Port = GetINIInt("GMES_INFO", "GMES_PORT", "7000");
            Timeout = GetINIInt("GMES_INFO", "GMES_TIMEOUT", "45");
            LinkTimeout = GetINIInt("GMES_INFO", "GMES_LINK_TIMEOUT", "10");
            m_bUseGMESInterface = Convert.ToBoolean(GetINIString("GMES_INFO", "USE_INTERFACE", "false"));


            // 2024.01.27 jh.kim 추가 - PLC read시 Retry 간격 및 횟수
            m_nRetryCount = GetINIInt("PLC_INFO", "RETRY_COUNT", "5");
            m_nRetryDelay = GetINIInt("PLC_INFO", "RETRY_Delay", "200");


            strCEID = GetINIString("GMES_DATA", "CEID", "20100");
            strRPTID = GetINIString("GMES_DATA", "RPTID", "20100");

            PLCIP = GetINIString("PLC_INFO", "PLC_IP", "127.0.0.1");
            PLC_Port = GetINIInt("PLC_INFO", "PLC_PORT", "8000");
            PLC_ReadStatus = GetINIString("PLC_INFO", "ReadStatus", "30600");
            PLC_WriteStatus = GetINIString("PLC_INFO", "WriteStatus", "30500");
            PLC_ReadBefInspData_A = GetINIString("PLC_INFO", "ReadBefInsptData_A", "32000");
            PLC_ReadBefInspData_B = GetINIString("PLC_INFO", "ReadBefInsptData_B", "42000");
            PLC_ReadAftInspData_A = GetINIString("PLC_INFO", "ReadAftInspData_A", "39000");
            PLC_ReadAftInspData_B = GetINIString("PLC_INFO", "ReadAftInspData_B", "49000");
            
            PLC_Area = GetINIString("PLC_INFO", "Area", "D");

            m_bUsePLCInterface = Convert.ToBoolean(GetINIString("PLC_INFO", "USE_INTERFACE", "false"));

            Slot = GetINIInt("HAIRPIN_INFO", "Slot", "96");
            Layer = GetINIInt("HAIRPIN_INFO", "Layer", "8");

            Slot77 = GetINIInt("HAIRPIN_INFO", "Slot77", "96");
            Layer77 = GetINIInt("HAIRPIN_INFO", "Layer77", "8");

            Slot76 = GetINIInt("HAIRPIN_INFO", "Slot76", "96");
            Layer76 = GetINIInt("HAIRPIN_INFO", "Layer76", "6");

            ProductLogPath = GetINIString("PATH", "ProductLogPath", "D:\\");

            // Jig A Product Info
            m_stProductInfoJigA.TID         = GetINIString("PRODUCT_INFO_JIGA", "TID", "");
            m_stProductInfoJigA.WorkID      = GetINIString("PRODUCT_INFO_JIGA", "WORKID", "");
            m_stProductInfoJigA.ORGID       = GetINIString("PRODUCT_INFO_JIGA", "ORGID", "");
            m_stProductInfoJigA.LineID      = GetINIString("PRODUCT_INFO_JIGA", "LINEID", "");
            m_stProductInfoJigA.model_ID    = GetINIString("PRODUCT_INFO_JIGA", "MODELID", "");
            m_stProductInfoJigA.model_Suffix  = GetINIString("PRODUCT_INFO_JIGA", "MODELSUFFIX", "");
            m_stProductInfoJigA.strStatorType = GetINIString("PRODUCT_INFO_JIGA", "STATORMODEL", "");

            // Jig B Product Info
            m_stProductInfoJigB.TID         = GetINIString("PRODUCT_INFO_JIGB", "TID", "");
            m_stProductInfoJigB.WorkID      = GetINIString("PRODUCT_INFO_JIGB", "WORKID", "");
            m_stProductInfoJigB.ORGID       = GetINIString("PRODUCT_INFO_JIGB", "ORGID", "");
            m_stProductInfoJigB.LineID      = GetINIString("PRODUCT_INFO_JIGB", "LINEID", "");
            m_stProductInfoJigB.model_ID    = GetINIString("PRODUCT_INFO_JIGB", "MODELID", "");
            m_stProductInfoJigB.model_Suffix  = GetINIString("PRODUCT_INFO_JIGB", "MODELSUFFIX", "");
            m_stProductInfoJigB.strStatorType = GetINIString("PRODUCT_INFO_JIGB", "STATORMODEL", "");

            // sjyi 2023.10.18 2차 용접기 여부 추가
            m_bRepairWeld = Convert.ToBoolean(GetINIString("HAIRPIN_INFO", "REPAIR_WELD", "false"));

            // sjyi 2024.01.14 헤어핀 회전을 위한 Equip ID 추가 --->
            m_nRotateEquipCnt = GetINIInt("HAIRPIN_INFO", "ROTATE_EQP_CNT", "0");

            m_listRotateEquipID = new List<string>(); // sjyi 2024.05.22 오류 수정

            string strKey = "";
            string strEqipID = "";
            for(int i=0; i< m_nRotateEquipCnt; i++)
            {
                strKey = string.Format("ROTATE_EQP_{0}", i);
                strEqipID = GetINIString("HAIRPIN_INFO", strKey, "");
                m_listRotateEquipID.Add(strEqipID);
            }
            // sjyi 2024.01.14 헤어핀 회전을 위한 Equip ID 추가 <---

            return nRtn;
        }

        //ini Write 하는 함수
        public bool WriteConfig()
        {
            bool nRtn = false;

            WritePrivateProfileString("GMES_INFO", "GMES_IP", GMES_IP, InIPath);
            WritePrivateProfileString("GMES_INFO", "GMES_PORT", GMES_Port.ToString(), InIPath);
            WritePrivateProfileString("GMES_INFO", "GMES_TIMEOUT", Timeout.ToString(), InIPath);
            WritePrivateProfileString("GMES_INFO", "GMES_LINK_TIMEOUT", LinkTimeout.ToString(), InIPath);
            WritePrivateProfileString("GMES_INFO", "USE_INTERFACE", m_bUseGMESInterface.ToString(), InIPath);


            WritePrivateProfileString("GMES_DATA", "CEID", strCEID, InIPath);
            WritePrivateProfileString("GMES_DATA", "RPTID", strRPTID.ToString(), InIPath);
            WritePrivateProfileString("PLC_INFO", "PLC_IP", PLCIP, InIPath);
            WritePrivateProfileString("PLC_INFO", "PLC_PORT", PLC_Port.ToString(), InIPath);
            WritePrivateProfileString("PLC_INFO", "ReadStatus", PLC_ReadStatus, InIPath);
            WritePrivateProfileString("PLC_INFO", "WriteStatus", PLC_WriteStatus, InIPath);            
            WritePrivateProfileString("PLC_INFO", "ReadBefInsptData_A", PLC_ReadBefInspData_A, InIPath);
            WritePrivateProfileString("PLC_INFO", "ReadBefInsptData_B", PLC_ReadBefInspData_B, InIPath);
            WritePrivateProfileString("PLC_INFO", "ReadAftInspData_A", PLC_ReadAftInspData_A, InIPath);
            WritePrivateProfileString("PLC_INFO", "ReadAftInspData_B", PLC_ReadAftInspData_B, InIPath);            
            WritePrivateProfileString("PLC_INFO", "Area", PLC_Area, InIPath);
            WritePrivateProfileString("PLC_INFO", "USE_INTERFACE", m_bUsePLCInterface.ToString(), InIPath);

            // 2024.01.27 jh.kim 추가 - PLC read시 Retry 간격 및 횟수
            WritePrivateProfileString("PLC_INFO", "RETRY_COUNT", m_nRetryCount.ToString(), InIPath);
            WritePrivateProfileString("PLC_INFO", "RETRY_DELAY", m_nRetryDelay.ToString(), InIPath);

            WritePrivateProfileString("HAIRPIN_INFO", "Slot", Slot.ToString(), InIPath);
            WritePrivateProfileString("HAIRPIN_INFO", "Layer", Layer.ToString(), InIPath);

            WritePrivateProfileString("HAIRPIN_INFO", "Slot77", Slot77.ToString(), InIPath);
            WritePrivateProfileString("HAIRPIN_INFO", "Layer77", Layer77.ToString(), InIPath);

            WritePrivateProfileString("HAIRPIN_INFO", "Slot76", Slot76.ToString(), InIPath);
            WritePrivateProfileString("HAIRPIN_INFO", "Layer76", Layer76.ToString(), InIPath);


            WritePrivateProfileString("PATH", "ProductLogPath", ProductLogPath, InIPath);

            // Jig A Product Info
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "TID", m_stProductInfoJigA.TID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "WORKID", m_stProductInfoJigA.WorkID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "ORGID", m_stProductInfoJigA.ORGID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "LINEID", m_stProductInfoJigA.LineID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "MODELID", m_stProductInfoJigA.model_ID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "MODELSUFFIX", m_stProductInfoJigA.model_Suffix, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGA", "STATORMODEL", m_stProductInfoJigA.strStatorType, InIPath);

            // Jig B Product Info
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "TID", m_stProductInfoJigB.TID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "WORKID", m_stProductInfoJigB.WorkID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "ORGID", m_stProductInfoJigB.ORGID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "LINEID", m_stProductInfoJigB.LineID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "MODELID", m_stProductInfoJigB.model_ID, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "MODELSUFFIX", m_stProductInfoJigB.model_Suffix, InIPath);
            WritePrivateProfileString("PRODUCT_INFO_JIGB", "STATORMODEL", m_stProductInfoJigB.strStatorType, InIPath);

            // sjyi 2023.10.18 2차 용접기 여부 추가
            WritePrivateProfileString("HAIRPIN_INFO", "REPAIR_WELD", m_bRepairWeld.ToString(), InIPath);

            // sjyi 2024.01.14 헤어핀 회전을 위한 Equip ID 추가 --->
            WritePrivateProfileString("HAIRPIN_INFO", "ROTATE_EQP_CNT", m_nRotateEquipCnt.ToString(), InIPath);

            string strKey = "";
            for (int i = 0; i < m_nRotateEquipCnt; i++)
            {
                strKey = string.Format("ROTATE_EQP_{0}", i);
                WritePrivateProfileString("HAIRPIN_INFO", strKey, m_listRotateEquipID[i], InIPath);
            }
            // sjyi 2024.01.14 헤어핀 회전을 위한 Equip ID 추가 <---

            return nRtn;
        }

        /// <summary>
        /// INI string 리턴 함수
        /// </summary>
        /// <param name="strSection">section 명</param>
        /// <param name="strKey">key 값</param>
        /// <param name="strdef">디폴트 값</param>
        /// <returns></returns>
        public string GetINIString(string strSection, string strKey, string strdef)
        {
            StringBuilder temp = new StringBuilder(255);
            GetPrivateProfileString(strSection, strKey, strdef, temp, 255, InIPath);

            if (temp.Length <= 0)
                return strdef;
            else
                return temp.ToString();
        }

        /// <summary>
        /// INI int 리턴 함수
        /// </summary>
        /// <param name="strSection">section 명</param>
        /// <param name="strKey">key 값</param>
        /// <param name="strdef">디폴트 값</param>
        /// <returns></returns>
        public int GetINIInt(string strSection, string strKey, string strdef)
        {
            StringBuilder temp = new StringBuilder(255);
            GetPrivateProfileString(strSection, strKey, strdef, temp, 255, InIPath);

            if (temp.Length <= 0)
                return int.Parse(strdef);
            else
                return int.Parse(temp.ToString());
        }


        public EPDS ReadProductData(int nJigNo)
        {
            ReadConfig();

            if(nJigNo == 0) // Jig A
            {
                return m_stProductInfoJigA;
            }
            else
            {
                return m_stProductInfoJigB;
            }
        }

        public void WriteProductData(int nJigNo, EPDS stProductInfo)
        {
            if (nJigNo == 0) // Jig A
            {
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "TID", stProductInfo.TID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "WORKID", stProductInfo.WorkID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "ORGID", stProductInfo.ORGID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "LINEID", stProductInfo.LineID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "MODELID", stProductInfo.model_ID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "MODELSUFFIX", stProductInfo.model_Suffix, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGA", "STATORMODEL", stProductInfo.strStatorType, InIPath);
            }
            else
            {
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "TID", stProductInfo.TID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "WORKID", stProductInfo.WorkID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "ORGID", stProductInfo.ORGID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "LINEID", stProductInfo.LineID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "MODELID", stProductInfo.model_ID, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "MODELSUFFIX", stProductInfo.model_Suffix, InIPath);
                WritePrivateProfileString("PRODUCT_INFO_JIGB", "STATORMODEL", stProductInfo.strStatorType, InIPath);
            }
        }

        // sjyi 2024.01.15 회전여부 검사, 기본값이 회전 안함(false)
        public bool CheckRotationEquip(string strEqpID)
        {
            if (m_nRotateEquipCnt <= 0) return false;

            for(int i = 0; i<m_nRotateEquipCnt; i++)
            {
                if(m_listRotateEquipID[i] == strEqpID)
                {
                    return true;
                }
            }

            return false;
        }
    }
}
