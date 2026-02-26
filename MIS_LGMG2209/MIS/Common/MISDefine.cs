using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MIS.Common
{
    public enum INPUT_FRAME {
        LINK = 0,
        STOP_SET = 1,
        STOP_RESET = 2
    };
    public enum OUTPUT_FRAME {
        LINK = 0,
        STOP_SET = 1,
        STOP_RESET = 2
    };

    //A 투입 보고, B 투입 보고, A 배출 보고, B 배출 보고, A 검사 데이터 결과, B 검사 데이터 결과
    public enum INPUT_RPFRAME {
        A_INPUT = 0,
        A_OUTPUT = 1,
        B_INPUT = 4,
        B_OUTPUT,
        A_PASS = 6,
        A_INJIG,
        B_INJIG,
        B_PASS = 9,
        A_BEFRET = 10,
        B_BEFRET,
        A_AFTRET = 13,
        B_AFTRET
    };

    public enum OUTPUT_RPFRAME {
        A_INPUT,
        A_OUTPUT,
        A_MODEL_ERROR,
        B_MODEL_ERROR,
        B_INPUT,
        B_OUTPUT,
        A_PASS = 6,
        A_INPUT_ERROR = 7,
        B_INPUT_ERROR,
        B_PASS = 9,
        A_BEFRET = 10,
        B_BEFRET,
        A_OUTPUT_ERROR,
        A_AFTRET = 13,
        B_AFTRET,
        B_OUTPUT_ERROR
    };

    //Model No(Job Num)
    public enum Model
    {
        Model_None = 0,
        Model_77A,
        Model_77B,
        Model_76A,
        Model_76B
    };

    //Model No
    public enum PLCModelNo
    {
        Model_None = 0,
        Model_77,
        Model_76
    }

    public enum eJigType
    {
        JigA = 1,
        JigB
    };

    //각 슬롯 레이버 결과 값
    public enum eJudgement
    {
        OK = 1,
        REWORK,
        NG,
        RW_OK,
        RW_NG
    };

    //최종 결과 값(전 검사 결과값도 같이 사용)
    public enum eFinalJudegment
    {
        OK = 1,
        NG
    };

    public enum eEuipmentSeq
    {
        EQP_INIT = 0,
        EQP_ONLINE,
        EQP_OFFLINE,
        EQP_OFFLINE_PLC,
    };

    public enum eACK
    {
        ACK_ACCEPTED = 0,
        ACK_NOTACCEPTED,
    }

    public class InspDataGMES
    {
        //최초 전 검사 데이터(Clamp Error에 대해 분석하기 위한 데이터)
        public string Slot = string.Empty;
        public string Layer = string.Empty;
        //public string H1 = "+" + string.Format("{0:f2}", 0);
        //public string H2 = "+" + string.Format("{0:f2}", 0);
        public string H1 = string.Format("{0:f2}", 0);
        public string H2 = string.Format("{0:f2}", 0);
        public string CValue = string.Format("{0:f2}", 0);
        public string Jump = string.Format("{0:f2}", 0);
        public string Theta = string.Format("{0:f2}", 0);

        //최종 전 검사 데이터(Bead 품질에 대해 분석하기 위한 데이터)
        //public string Judge_H1 = "+" + string.Format("{0:f2}", 0);
        //public string Judge_H2 = "+" + string.Format("{0:f2}", 0);
        public string Judge_H1 = string.Format("{0:f2}", 0);
        public string Judge_H2 = string.Format("{0:f2}", 0);
        public string Judge_CValue = string.Format("{0:f2}", 0);
        public string Judge_Jump = string.Format("{0:f2}", 0);
        public string Judge_Theta = string.Format("{0:f2}", 0);

        //최종 결과 값(Deep learning N차 결과 저장 하기 위한 데이터)
        //public string Judgement = string.Empty;
        public List<string> LsJudgement = new List<string>();

        //현재 안쓰는 데이터(추 후 BeforeInsptDataParsing() 함수에서 필요한 데이터 쓰면 됨)
        public string Result = string.Empty;
        public string H3 = string.Empty;
        public string A1 = string.Empty;
        public string A2 = string.Empty;
        public string aX = string.Empty;
        public string aY = string.Empty;
        public string bX = string.Empty;
        public string bY = string.Empty;

        //용접 조건 및 용접 횟수()
        public string Percentage = string.Empty;
        public string WeldCount = string.Empty;

        public WeldingParam WeldParam = new WeldingParam();
    }

    public class WeldingParam
    {
        //1차용접
        public string Inner_Power = "0";
        public string Inner_WeldSpd = "0";
        public string Inner_Repeat = "0";
        public string Outer_Power = "0";
        public string Outer_WeldSpd = "0";
        public string Outer_Repeat = "0";

        //2차 용접 
        public string Whole_Power = "0";
        public string Whole_WeldSpd = "0";
        public string Whole_Repeat = "0";

        //용접 횟수
        public string WeldCount = "0";
    }

    // Are You There Ack 정보(MES상 Equipment 정보)
    public class EAYT_R
    {
        public string EQPID = "Equipment ID";
        public string EQPName = "Equipment Name";
        public string ProcID = "Process ID";
        public string ORGID = "Organization ID";
        public string LineID = "Line ID";
    }

    // Equipment Product Data Send 정보(MES상 Equipment 정보)
    public class EPDS
    {
        public string TID = "";
        public string WorkID = "";
        public string ORGID = "";
        public string LineID = "";
        public string SetID = "";

        public string model_ID = "";
        public string model_Name = "";
        public string model_Suffix = "";

        public List<ProductSubItem> LsSubItem = new List<ProductSubItem>();

        public string strStatorType = ""; // 77 Type : 12, 76 Type : 11
    }

    public class ProductSubItem
    {
        public string strName = string.Empty;
        public string strValue = string.Empty;
    }

    public class EERS_R
    {
        public string EQPID;
        public string CEID;
        public string RPTID;
        public string ACK;
        public string REASON;

        public eJigType nJigType;
    }

    public class EIDS_R
    {
        public string SETID;
        public string ACK;
        public string REASON;
    }

    public struct ExtInfo
    {
        public short LoadRate_HeadX { get; set; }
        public short LoadRate_HeadZ { get; set; }
        public short LoadRate_ClampY { get; set; }
        public short LoadRate_ClampZ { get; set; }
        public short LoadRate_ClampC { get; set; }
        public short LoadRate_CenteringZ { get; set; }
    }
}
