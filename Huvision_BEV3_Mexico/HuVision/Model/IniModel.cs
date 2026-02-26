using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HuVision.Model
{
    public class IniModel
    {
        public int Process_Sleep { get; set; }
        public string Process_Name { get; set; }
        public string Process_SavePath { get; set; }
        public bool Process_Manual { get; set; }        
        public int Process_AfterTimeOut { get; set; }
        public bool Process_PLCWrite { get; set; }
        public int Process_ResultStandard { get; set; }     // 0 = Rework, 1 = Fail              
        public bool Process_ReadRework { get; set; } // sjyi 2023.10.17 1차용접기, 2차용접기용 구분
        public bool Process_MakePreData { get; set; } // sjyi 2024.06.07 추가, 이전 검사 데이터 생성 여부

        public int Model77_Slot { get; set; }
        public int Model77_Layer { get; set; }
        public float Model77_Range { get; set; }
        public float Model77_Angle { get; set; }
        public float Model77_XOffSet { get; set; }
        public float Model77_YOffSet { get; set; }

        public int Model76_Slot { get; set; }
        public int Model76_Layer { get; set; }
        public float Model76_Range { get; set; }
        public float Model76_Angle { get; set; }
        public float Model76_XOffSet { get; set; }
        public float Model76_YOffSet { get; set; }

        public string PLC_IP { get; set; }
        public int PLC_Port { get; set; }
        public string PLC_Area { get; set; }

        public string PLC_ReadStatus { get; set; }
        public string PLC_WriteStatus { get; set; }
        
        //public string PLC_ReadInspectionData { get; set; }  // 전검사 데이터 
        public string PLC_WriteReWorkData { get; set; }     // 후검사 데이터

		// sjyi 2023.07.20 재용접포인트 Read 주소 별도 지정
        public string PLC_ReadReWorkData { get; set; }

        public string PLC_ReadAfterPath { get; set; }       // 후검사 경로
        public string PLC_WriteAfterResult { get; set; }    // 후 검사 결과 (0 = None, 1 = OK, 2 = NG, 3 = Aalarm)        

        public string Coulmn_Col1 { get; set; }
        public string Coulmn_Col2 { get; set; }
        public string Coulmn_Col3 { get; set; }
        public string Coulmn_Col4 { get; set; }
        public string Coulmn_Col5 { get; set; }
        public string Coulmn_Col6 { get; set; }
        public string Coulmn_Col7 { get; set; }
        public string Coulmn_Col8 { get; set; }
        public string Coulmn_Col9 { get; set; }
        public string Coulmn_Col10 { get; set; }

        public bool DB_Use { get; set; }
        public string DB_IP { get; set; }
        public string DB_Catagory { get; set; }
        public string DB_ID { get; set; }
        public string DB_PW { get; set; }
    }
}
