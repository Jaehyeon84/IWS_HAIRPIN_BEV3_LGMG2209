using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using System.Windows.Forms;
using HubisCommonControl;  // 휴비스 공통 클래스 사용하기 위해 선언(Log, Socket 등)
using System.Collections;
using MIS.Common;

namespace MIS.Communication
{
    class PLCSocket
    {
        // 멤버변수  -----------------------------------------------
        // PLC 통신 소켓 클래스
        protected MelsecSock m_MelsecSock = null;

        // PLC Thread 함수
        protected Thread m_PLCThread;
        protected bool m_bThreadRun = false;

        //D30600 PLC Read Status 저장하는 변수(PLC -> PC)
        protected short[] inputSignal = new short[15];

        //D30500 PLC Write Status 저장하는 변수(PC -> PLC)
        protected short[] outputSignal = new short[15];

        //지그 별 배출, 투입보고 저장하는 변수
        protected short[] inputReportSignal = new short[15];

        // 데이터 수신용 버버
        protected BitArray bitArray;

        protected BitArray bitArrayReport;

        //지그 별 배출, 투입보고 및  최초 전 검사 데이터 Read Response 저장하는 변수
        protected short[] outputReportSignal = new short[16];

        // 1차장비 최종 검사 결과
        protected short m_n1stResult = 0;

        //output 상태를 PLC에 쓰기 위한 데이터
        protected List<byte> writePlcLinkList = new List<byte>();
        protected List<byte> writePlcList = new List<byte>();

        protected int nLinkTestCnt = 0;

        protected string strModelNum = string.Empty;

        //최종 판정 값
        protected string strFinalJudgement_A = string.Empty;
        protected string strFinalJudgement_B = string.Empty;

        //용접 횟수
        protected int nReworkCnt_A = 0;
        protected int nReworkCnt_B = 0;

        protected string ReadBefInspData = string.Empty;
        protected string ReadAftInspData = string.Empty;
        protected string ReadPercentData = string.Empty;

        //12 WORD (24 자리)
        protected StringBuilder LotID_A = new StringBuilder("BARCODE2000000000000000A");
        protected StringBuilder LotID_B = new StringBuilder("BARCODE2000000000000000B");

        //PLC에서 읽어온 전 검사 데이터(최초, 최종 전 검사 데이터)
        protected List<byte> LsPlcBefInsptDataBytes = new List<byte>();          // Byte Data
        //PLC에서 읽어온 전 검사 데이터 셋팅
        protected List<short> LsPlcBefInsptDataWORDS = new List<short>();        // WORD Data

        //PLC에서 읽어온 후 검사 데이터
        protected List<byte> LsPlcAftInsptDataBytes= new List<byte>();          // Byte Data
        //PLC에서 읽어온 후 검사 데이터 셋팅
        protected List<short> LsPlcAftInsptDataWORDS = new List<short>();        // WORD Data

        // 검사 데이터(MES 보고용)
        protected List<InspDataGMES> lstGMESInsptDataAJig = new List<InspDataGMES>();
        protected List<InspDataGMES> lstGMESInsptDataBJig = new List<InspDataGMES>();

        // Ini 파일 클래스
        protected ConfigINI m_cConfig = new ConfigINI();

        // Product가 Jig에 있는지 확인용 Flag
        protected bool m_bInProductJigA = false;
        protected bool m_bInProductJigB = false;

        // Output Bit Edge Detect용 변수
        protected bool m_bOutputOldJigA = false;
        protected bool m_bOutputOldJigB = false;

        // Input Bit Edge Detect용 변수
        protected bool m_bInputOldJigA = false;
        protected bool m_bInputOldJigB = false;

        protected int m_nMaxPLCSend = 100;

        // 멤버함수  -----------------------------------------------
        // 생성자
        public PLCSocket()
        {
            // Get Ini Info & Set Info
            string strPath = Application.StartupPath + "\\MISConfig.ini";
            string strExe = Application.StartupPath + "\\";
            m_cConfig.InIPath = strPath;
            m_cConfig.ReadConfig();
            m_cConfig.WriteConfig();

            // PLC Socket Create
            m_MelsecSock = new MelsecSock(m_cConfig.PLCIP, m_cConfig.PLC_Port);
        }

        // 외부 공개 함수 ----------------------------------------------------
        // 초기화 함수
        public void Intialize()
        {
            // Thrad Start
            PLCThreadStart();
        }

        // 종료 함수
        public void Terminate()
        {
            PLCThreadStop();

            if (m_MelsecSock != null)
            {
                m_MelsecSock.isClose = true;
                m_MelsecSock.Close();
                m_MelsecSock = null;
            }
        }

        public bool GetIsConnected()
        {
            return m_MelsecSock.IsConnected;
        }

        // 2024.02.12 jh.kim 추가 - Repair 구분
        public bool GetRepairWeld()
        {
            return m_cConfig.m_bRepairWeld;
        }

        // 쓰레드 시작 함수(PLC Read / Write)
        protected void PLCThreadStart()
        {
            m_PLCThread = new Thread(new ThreadStart(PLCThreadProcess));
            m_bThreadRun = true;
            m_PLCThread.Start();
        }

        // 쓰레드 종료 함수
        protected void PLCThreadStop()
        {
            m_bThreadRun = false;
            m_PLCThread.Abort();
            m_PLCThread = null;
        }

        // 쓰레드 함수
        protected void PLCThreadProcess()
        {
            while (m_bThreadRun)
            {
                // Read Requst Data
                ReadPLCRequstData();

                // Write Response Data
                WritePLCResponseData();

                Thread.Sleep(10);
            }
        }

        // Bit Array를 Word로 변경
        protected short GetBitToUshort(bool value)
        {
            if (value == true)
                return (short)1;
            else
                return (short)0;
        }

        // PLC Data(WORD)를 실수형으로 변환
        protected double GetTruncateWord2Double(double value)
        {
            return Math.Truncate(value * 100) / 100;
        }

        // PLC -> PC Data(Request) Read
        protected bool ReadPLCRequstData()
        {
            if (m_MelsecSock.IsConnected == false)
            {
                return false;
            }

            // Read From PLC
            byte[] rcvData = m_MelsecSock.Read(m_cConfig.PLC_Area, int.Parse(m_cConfig.PLC_ReadStatus), 39);

            if (rcvData == null || rcvData.Length != 78)
            {
                return false;
            }

            //D30600 - Link Test
            //1WORD = 2Byte = 16bit
            bitArray = new BitArray(new byte[] { rcvData[0], rcvData[1] });

            inputSignal[(int)INPUT_FRAME.LINK] = GetBitToUshort(bitArray[0]);     //LinkTest Request

            //D30601 - 각 지그별 투입, 배출 보고 Status
            bitArrayReport = new BitArray(new byte[] { rcvData[2], rcvData[3] });

            inputReportSignal[(int)INPUT_RPFRAME.A_INPUT] = GetBitToUshort(bitArrayReport[0]);   //A Jig 투입보고 Request
            inputReportSignal[(int)INPUT_RPFRAME.A_OUTPUT] = GetBitToUshort(bitArrayReport[1]);  //A Jig 배출보고 Request
            inputReportSignal[(int)INPUT_RPFRAME.B_INPUT] = GetBitToUshort(bitArrayReport[4]);   //B Jig 투입보고 Request
            inputReportSignal[(int)INPUT_RPFRAME.B_OUTPUT] = GetBitToUshort(bitArrayReport[5]);  //B Jig 배출보고 Request            

            inputReportSignal[(int)INPUT_RPFRAME.A_PASS] = GetBitToUshort(bitArrayReport[6]);  //A Jig Pass Request
            inputReportSignal[(int)INPUT_RPFRAME.B_PASS] = GetBitToUshort(bitArrayReport[9]);  //B Jig Pass Request

            inputReportSignal[(int)INPUT_RPFRAME.A_INJIG] = GetBitToUshort(bitArrayReport[7]);  //A In Jig Flag(제품이 있는지 확인 Bit)
            inputReportSignal[(int)INPUT_RPFRAME.B_INJIG] = GetBitToUshort(bitArrayReport[8]);  //B In Jig Flag(제품이 있는지 확인 Bit)

            //전 검사 데이터 결과 값(최초 점 검사 데이터 읽어오기 위한 시점일뿐 실제 NG, OK에 대한 결과 값은 필요하지 않음)
            inputReportSignal[(int)INPUT_RPFRAME.A_BEFRET] = GetBitToUshort(bitArrayReport[10]); //A Jig 점 검사 데이터 결과
            inputReportSignal[(int)INPUT_RPFRAME.B_BEFRET] = GetBitToUshort(bitArrayReport[11]); //B Jig 점 검사 데이터 결과

            //후 검사 결과 데이터 Read 시점
            inputReportSignal[(int)INPUT_RPFRAME.A_AFTRET] = GetBitToUshort(bitArrayReport[13]);
            inputReportSignal[(int)INPUT_RPFRAME.B_AFTRET] = GetBitToUshort(bitArrayReport[14]);

            //2022.09.09 PJM
            //Model Num(PLC에서 현재 배출하려고 하는 제품이 A, B Jig인지 알 수 있는 방법이 없으므로 배출 보고 시 PLC 신호로(배출보고 요청 신호) 구분하고)
            //77인지 76인지 Model만 구분
            //0 = None, 1 = 77, 2 = 76
            short ModelNum = BitConverter.ToInt16(rcvData, 10);
            switch (ModelNum)
            {
                case 0: strModelNum = "0"/*PLCModelNo.Model_None.ToString()*/; break;
                case 1: strModelNum = "1"/*PLCModelNo.Model_77.ToString()*/; break;
                case 2: strModelNum = "2"/*PLCModelNo.Model_76.ToString()*/; break;
                default:
                    strModelNum = "0"/*Model.Model_None.ToString()*/;
                    LogManager.WriteLog(LogType.Error, "Currently, the Model type is not included in the Model type[" + ModelNum + "].");
                    break;
            }

            // 모델명에 따라 Slot, Layer 개수 Update
            UpdateSlotnLayerCnt(strModelNum);

            //최종 판정 값(JIG A)
            strFinalJudgement_A = string.Empty;
            short sJudgement = BitConverter.ToInt16(rcvData, 12);
            strFinalJudgement_A = (sJudgement < Convert.ToInt16(eFinalJudegment.NG) ? eFinalJudegment.OK.ToString() : eFinalJudegment.NG.ToString());

            //최종 판정 값(JIG B)
            sJudgement = BitConverter.ToInt16(rcvData, 14);
            strFinalJudgement_B = (sJudgement < Convert.ToInt16(eFinalJudegment.NG) ? eFinalJudegment.OK.ToString() : eFinalJudegment.NG.ToString());

            //용접 횟수
            nReworkCnt_A = BitConverter.ToInt16(rcvData, 16);
            nReworkCnt_B = BitConverter.ToInt16(rcvData, 18);

            //LotID
            //PLC에서 BarCode Reading 시 15 word PLC 영역에 Write 하지만 앞에 4word는 쓰레기 값이므로 제외하고 11word 읽어야함
            LotID_A.Clear();
            LotID_A.Append(Encoding.Default.GetString(rcvData.Skip(20).Take(11 * 2).ToArray()).Trim('\0'));

            LotID_B.Clear();
            LotID_B.Append(Encoding.Default.GetString(rcvData.Skip(50).Take(11 * 2).ToArray()).Trim('\0'));

            return true;
        }

        // 모델명에 따라 Slot 개수, Layer 개수 Update
        public void UpdateSlotnLayerCnt(string strModel)
        {
            //Slot, Layer
            //76F 하부 : 96 Slot 6 Layer
            //    씨링 : 12 Slot 4 Layer 
            //77R 하부 : 96 Slot 8 Layer
            //    씨링 : 24 Slot 4 Layer

            if (strModel == "1") // 77
            {
                m_cConfig.Slot = m_cConfig.Slot77;
                m_cConfig.Layer = m_cConfig.Layer77;
            }
            else if (strModel == "2") // 76
            {
                m_cConfig.Slot = m_cConfig.Slot76;
                m_cConfig.Layer = m_cConfig.Layer76;
            }
            else
            {
                m_cConfig.Slot = m_cConfig.Slot77;
                m_cConfig.Layer = m_cConfig.Layer77;
            }
        }

        // PC -> PLC Data(Response) Write
        protected bool WritePLCResponseData()
        {
            if (m_MelsecSock.IsConnected == false)
            {
                return false;
            }

            byte[] sndData = GetOutputResponse();

            WriteToPLC(int.Parse(m_cConfig.PLC_WriteStatus), sndData.Length / 2, sndData);

            return true;
        }

        // PC -> PLC 비트 값을 byte 배열로 변경
//        protecte byte[] GetOutputResponse()
        public byte[] GetOutputResponse()
        {
            writePlcList.Clear();

            ushort tmpWORD1 = Convert.ToUInt16(string.Format("000000000000000{0}", outputSignal[(int)OUTPUT_FRAME.LINK]));
            string strOutReportSignal = string.Format("{15}{11}{10}{14}{9}{8}{13}{7}{6}{12}{5}{4}{3}{2}{1}{0}", outputReportSignal[(int)OUTPUT_RPFRAME.A_INPUT],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_OUTPUT],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_MODEL_ERROR],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_MODEL_ERROR],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_INPUT],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_OUTPUT],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_INPUT_ERROR],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_INPUT_ERROR],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_BEFRET],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_BEFRET],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_AFTRET],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_AFTRET],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_PASS],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_PASS],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.A_OUTPUT_ERROR],
                                                                                                           outputReportSignal[(int)OUTPUT_RPFRAME.B_OUTPUT_ERROR]);

            ushort tmpWORD2 = Convert.ToUInt16(strOutReportSignal, 2);
            ushort tmpWORD3 = 0;
            ushort tmpWORD4 = 0;
            ushort tmpWORD5 = 0;
            ushort tmpWORD6 = (ushort)m_n1stResult;

            writePlcList.AddRange(BitConverter.GetBytes(tmpWORD1));
            writePlcList.AddRange(BitConverter.GetBytes(tmpWORD2));
            writePlcList.AddRange(BitConverter.GetBytes(tmpWORD3));
            writePlcList.AddRange(BitConverter.GetBytes(tmpWORD4));
            writePlcList.AddRange(BitConverter.GetBytes(tmpWORD5));
            writePlcList.AddRange(BitConverter.GetBytes(tmpWORD6));

            return writePlcList.ToArray();
        }

        // PLC->PC Bit값 확인 함수 ----------------------------------------------------
        // PLC Link Test
        public bool GetLink()
        {
            return Convert.ToBoolean(inputSignal[(int)INPUT_FRAME.LINK]);
        }

        //A Jig 전 검사 결과 Req
        public bool GetBeforeInspectA()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.A_BEFRET]);
        }

        //B Jig 전 검사 결과 Req
        public bool GetBeforeInspectB()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.B_BEFRET]);
        }

        //A Jig 후 검사 결과 Req
        public bool GetAfterInspectA()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.A_AFTRET]);
        }

        //B Jig 후 검사 결과 Req
        public bool GetAfterInspectB()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.B_AFTRET]);
        }

        //A Jig 제품 투입 Req
        public bool GetInputA()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.A_INPUT]);
        }

        //B Jig 제품 투입 Req
        public bool GetInputB()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.B_INPUT]);
        }

        public bool GetInputOldA()
        {
            return m_bInputOldJigA;
        }

        public bool GetInputOldB()
        {
            return m_bInputOldJigB;
        }

        public void SetInputOldA(bool bValue)
        {
            m_bInputOldJigA = bValue;
        }

        public void SetInputOldB(bool bValue)
        {
            m_bInputOldJigB = bValue;
        }

        //A Jig 제품 배출 Req
        public bool GetOutputA()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.A_OUTPUT]);
        }

        //B Jig 제품 배출 Req
        public bool GetOutputB()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.B_OUTPUT]);
        }

        public bool GetOutputOldA()
        {
            return m_bOutputOldJigA;
        }

        public bool GetOutputOldB()
        {
            return m_bOutputOldJigB;
        }

        public void SetOutputOldA(bool bValue)
        {
            m_bOutputOldJigA = bValue;
        }

        public void SetOutputOldB(bool bValue)
        {
            m_bOutputOldJigB = bValue;
        }

        //A Jig Pass Req
        public bool GetPassA()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.A_PASS]);
        }

        //B Jig Pass Req
        public bool GetPassB()
        {
            return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.B_PASS]);
        }

        // PC->PLC Bit값 확인 함수 (Event 감지용)----------------------------------------------------
        public bool GetLinkTestRes()
        {
            return Convert.ToBoolean(outputSignal[(int)OUTPUT_FRAME.LINK]);
        }

        //A Jig 제품 투입 Response
        public bool GetInputARes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_INPUT]);
        }

        //B Jig 제품 투입 Response
        public bool GetInputBRes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_INPUT]);
        }

        //A Jig 제품 배출 Response
        public bool GetOutputARes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_OUTPUT]);
        }

        //B Jig 제품 배출 Response
        public bool GetOutputBRes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_OUTPUT]);
        }

        //A Jig 전 검사 Read Response
        public bool GetBeforeInspectARes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_BEFRET]);
        }

        //B Jig 전 검사 Read Response
        public bool GetBeforeInspectBRes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_BEFRET]);
        }

        //A Jig 후 검사 Read Response
        public bool GetAfterInspectARes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_AFTRET]);
        }

        //B Jig 전 검사 Read Response
        public bool GetAfterInspectBRes()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_AFTRET]);
        }

        public bool GetInputErrorA()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_INPUT_ERROR]);
        }

        public bool GetInputErrorB()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_INPUT_ERROR]);
        }

        // PASS
        public bool GetPassResA()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_PASS]);
        }

        public bool GetPassResB()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_PASS]);
        }

        public bool GetOutputErrorA()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.A_OUTPUT_ERROR]);
        }

        public bool GetOutputErrorB()
        {
            return Convert.ToBoolean(outputReportSignal[(int)OUTPUT_RPFRAME.B_OUTPUT_ERROR]);
        }


        // PC->PLC Bit값 설정 함수 ----------------------------------------------------
        //A Jig 제품 모델 설정 Error
        public void Set_A_Model_Error(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_MODEL_ERROR] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Model Error ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Model Error OFF");
            }
        }

        //B Jig 제품 모델 설정 Error
        public void Set_B_Model_Error(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_MODEL_ERROR] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Model Error ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Model Error OFF");
            }
        }

        //A Jig 후 검사 결과 Response
        public void SetAfterInspectARes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_AFTRET] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : After Inspect Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : After Inspect Response OFF");
            }
        }

        //B Jig 후 검사 결과 Response
        public void SetAfterInspectBRes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_AFTRET] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : After Inspect Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : After Inspect Response OFF");
            }
        }

        //A Jig 전 검사 결과 Response
        public void SetBeforeInspectARes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_BEFRET] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Before Inspect Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Before Inspect Response OFF");
            }
        }

        //B Jig 전 검사 결과 Response
        public void SetBeforeInspectBRes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_BEFRET] = GetBitToUshort(isFlag);
            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Before Inspect Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Before Inspect Response OFF");
            }
        }

        //A Jig 투입 Response
        public void SetInputARes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_INPUT] = GetBitToUshort(isFlag);

            // Logging
            if(isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Input Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Input Response OFF");
            }
        }

        //B Jig 투입 Response
        public void SetInputBRes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_INPUT] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Input Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Input Response OFF");
            }
        }

        //A Jig 배출 Response
        public void SetOutputARes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_OUTPUT] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Output Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Output Response OFF");
            }
        }

        //B Jig 배출 Response
        public void SetOutputBRes(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_OUTPUT] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Output Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Output Response OFF");
            }
        }

        //A Jig 투입 바코드 Error
        public void SetInputErrorA(bool bFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_INPUT_ERROR] = GetBitToUshort(bFlag);

            // Logging
            if (bFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Input Error ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Input Error OFF");
            }
        }

        //B Jig 투입 바코드 Error
        public void SetInputErrorB(bool bFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_INPUT_ERROR] = GetBitToUshort(bFlag);

            // Logging
            if (bFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Input Error ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Input Error OFF");
            }
        }

        //A Jig PASS Response
        public void SetPassResA(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_PASS] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Pass Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Pass Response OFF");
            }
        }

        //B Jig PASS Response
        public void SetPassResB(bool isFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_PASS] = GetBitToUshort(isFlag);

            // Logging
            if (isFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Pass Response ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Pass Response OFF");
            }
        }

        //A Jig 배출 Error
        public void SetOutputErrorA(bool bFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.A_OUTPUT_ERROR] = GetBitToUshort(bFlag);

            // Logging
            if (bFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Output Error ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig A : Output Error OFF");
            }
        }

        //B Jig 배출 Error
        public void SetOutputErrorB(bool bFlag)
        {
            outputReportSignal[(int)OUTPUT_RPFRAME.B_OUTPUT_ERROR] = GetBitToUshort(bFlag);

            // Logging
            if (bFlag == true)
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Output Error ON");
            }
            else
            {
                LogManager.WriteLog(LogType.Information, "[PC->PLC] Jig B : Output Error OFF");
            }
        }

        // PLC Link Test
        public void SetLink(bool isFlag)
        {
            outputSignal[(int)OUTPUT_FRAME.LINK] = GetBitToUshort(isFlag);
        }

        // PC->PLC Word값 확인 함수 ----------------------------------------------------
        // PLC Model
        public string GetPLCModel()
        {
            return strModelNum;
        }

        // 최종판정 Jig A
        public string GetFinalJudgeA()
        {
            return strFinalJudgement_A;
        }

        // 최종판정 Jig B
        public string GetFinalJudgeB()
        {
            return strFinalJudgement_B;
        }

        // Rework Counter Jig A
        public int GetReworkCntA()
        {
            return nReworkCnt_A;
        }

        // Rework Counter Jig B
        public int GetReworkCntB()
        {
            return nReworkCnt_B;
        }

        // Barcode(LOTID) Jig A
        public string GetBarcodeA()
        {
            return LotID_A.ToString();
        }

        // Barcode(LOTID) Jig B
        public string GetBarcodeB()
        {
            return LotID_B.ToString();
        }

        // PLC->PC 검사 결과 확인 함수 ----------------------------------------------------
        // 전검사 결과 Update
        public void ReadBefInspect(int nJibNo, bool bFist)
        {
            // Check Connected Status
            if (m_MelsecSock.IsConnected == false)
            {
                return;
            }

            // PLC Read Start(Logging)
            LogManager.WriteLog(LogType.Information, "Get Before InspectionData Start");

            // PLC Read Address
            int nPLCAddress = 0;
            if (nJibNo == 0) // Jig A
            {
                nPLCAddress = int.Parse(m_cConfig.PLC_ReadBefInspData_A);
            }
            else // Jig B
            {
                nPLCAddress = int.Parse(m_cConfig.PLC_ReadBefInspData_B);
            }

            // 전체 Layer 개수
            int nTotalLayerCnt = m_cConfig.Slot * (m_cConfig.Layer / 2); // Slot * Layer(용접 전 레이어) / 2 = 전체 Layer 개수
            // 전체 Word 개수
            int nWordTatal = nTotalLayerCnt * 16; //전체 Layer 개수 * 16(항목 개수) = 전체 WORD Size
            // 반복 횟수 (16번)
            int loopCount = (int)Math.Ceiling(nWordTatal / (double)nTotalLayerCnt);
            // 임시버퍼 사이즈
            int tmpWord = nWordTatal;
            // PLC Read 임시 버퍼
            byte[] tmpbytes = null;

            // 임시버퍼 사이즈 만큼 PLC Read 반복
            LsPlcBefInsptDataBytes.Clear();
            tmpWord = Math.Min(nWordTatal, nTotalLayerCnt);

            // 2024.01.26 jh.kim 추가 - PLC read시 Retry 간격 및 횟수
            int iRetryCount = 0;
            bool bReadOk = false;
            while (bReadOk == false)
            {
                try
                {
		            for (int i = 0; i < loopCount; i++)
		            {
		                tmpbytes = m_MelsecSock.Read(m_cConfig.PLC_Area, nPLCAddress, tmpWord);
		                LsPlcBefInsptDataBytes.AddRange(tmpbytes);
		                bReadOk = true;
		            }
                }
                catch (Exception ex)
                {
                    if (iRetryCount > m_cConfig.m_nRetryCount)
                    {
                        LogManager.WriteLog(LogType.Information, "GetInspection Data Read Error");
                        bReadOk = true;;
                    }

                    Thread.Sleep(m_cConfig.m_nRetryDelay);
                    iRetryCount++;
                }
                
            }

            // Convert Bytes to WORDS
            LsPlcBefInsptDataWORDS .Clear();
            for (int i = 0; i < LsPlcBefInsptDataBytes.ToArray().Length; i += 2)
                LsPlcBefInsptDataWORDS .Add(BitConverter.ToInt16(LsPlcBefInsptDataBytes.ToArray(), i));

            // PLC Read End(Logging)
            LogManager.WriteLog(LogType.Information, "Get Before InspectionData End");

            // Buffer to GMES Data
            if (nJibNo == 0) // Jig A
            {
                ConverBeforeInspectWordToGMESData(LsPlcBefInsptDataWORDS, lstGMESInsptDataAJig, bFist);
            }
            else
            {
                ConverBeforeInspectWordToGMESData(LsPlcBefInsptDataWORDS, lstGMESInsptDataBJig, bFist);
            }
        }

        // 전검사 결과 WORD값을 MES용 데이터로 변환
        protected bool ConverBeforeInspectWordToGMESData(List<short> lstRawData, List<InspDataGMES> lstInsptData, bool b1stFlag)
        {
            if (lstRawData.Count == 0)
            {
                return false;
            }

            if (b1stFlag == true || lstInsptData.Count() == 0)
            {
                lstInsptData.Clear();
                for (int i = 0; i < m_cConfig.Slot; i++)
                {
                    for (int j = 0; j < (m_cConfig.Layer / 2); j++)
                    {
                        InspDataGMES tmp = new InspDataGMES();
                        //22.07.03 1_1-2 -> 01_1-2 format 변경으로 인한 처리
                        tmp.Slot = (i + 1).ToString("D2");
                        switch (j)
                        {
                            case 0: tmp.Layer = "1-2"; break;
                            case 1: tmp.Layer = "3-4"; break;
                            case 2: tmp.Layer = "5-6"; break;
                            case 3: tmp.Layer = "7-8"; break;
                        }
                        lstInsptData.Add(tmp);
                    }
                }
            }

            //전 검사 데이터(PLC로부터 Read), i += 16(데이터 항목 갯수)            
            //모델 명 및 C-Ring 같은경우 Layer가 1~8 Layer 까지 존재하지 않으므로 위에 Layer, Slot 매핑하는 로직에서 매핑 후, Layer가 매핑 된 데이터만 처리
            int nIndex = 0;
            for (int j = 0; j < lstRawData.Count() - 1; j += 16)
            {

                //Result 0 = None, 1 = OK, 2 = NG
                //전 검사 결과가 0일 경우, 검사를 하지 않았다고 판단 하여 bBefInsptFlagA(최초 전 검사 데이터 Read 하는 flag 값) flag 값 변경하지 않는다
                if (lstInsptData.Count == 0)
                    return false;

                //GMES Server Data format 맞춤
                if (b1stFlag) // 최초 전검사 데이터
                {
                    if (GetTruncateWord2Double(double.Parse(lstRawData[j + 1].ToString()) / 100) < 0)
                        lstInsptData[nIndex].H1 = string.Format("{0:f2}", GetTruncateWord2Double(double.Parse(lstRawData[j + 1].ToString()) / 100));
                    else
                        lstInsptData[nIndex].H1 = "+" + string.Format("{0:f2}", GetTruncateWord2Double(double.Parse(lstRawData[j + 1].ToString()) / 100));

                    if (GetTruncateWord2Double(double.Parse(lstRawData[j + 2].ToString()) / 100) < 0)
                        lstInsptData[nIndex].H2 = string.Format("0:f2", GetTruncateWord2Double(double.Parse(lstRawData[j + 2].ToString()) / 100));
                    else
                        lstInsptData[nIndex].H2 = "+" + string.Format("{0:f2}", GetTruncateWord2Double(double.Parse(lstRawData[j + 2].ToString()) / 100));

                    lstInsptData[nIndex].CValue = string.Format("{0:f2}", Math.Abs(GetTruncateWord2Double(double.Parse(lstRawData[j + 10].ToString()) / 100)));
                    lstInsptData[nIndex].Jump = string.Format("{0:f2}", Math.Abs(GetTruncateWord2Double(double.Parse(lstRawData[j + 11].ToString()) / 100)));
                    lstInsptData[nIndex].Theta = string.Format("{0:f2}", Math.Abs(GetTruncateWord2Double(double.Parse(lstRawData[j + 12].ToString()) / 100)));
                }
                else // 최종 전검사 데이터
                {
                    if (GetTruncateWord2Double(double.Parse(lstRawData[j + 1].ToString()) / 100) < 0)
                        lstInsptData[nIndex].Judge_H1 = string.Format("{0:f2}", GetTruncateWord2Double(double.Parse(lstRawData[j + 1].ToString()) / 100));
                    else
                        lstInsptData[nIndex].Judge_H1 = "+" + string.Format("{0:f2}", GetTruncateWord2Double(double.Parse(lstRawData[j + 1].ToString()) / 100));

                    if (GetTruncateWord2Double(double.Parse(lstRawData[j + 2].ToString()) / 100) < 0)
                        lstInsptData[nIndex].Judge_H2 = string.Format("0:f2", GetTruncateWord2Double(double.Parse(lstRawData[j + 2].ToString()) / 100));
                    else
                        lstInsptData[nIndex].Judge_H2 = "+" + string.Format("{0:f2}", GetTruncateWord2Double(double.Parse(lstRawData[j + 2].ToString()) / 100));

                    lstInsptData[nIndex].Judge_CValue = string.Format("{0:f2}", Math.Abs(GetTruncateWord2Double(double.Parse(lstRawData[j + 10].ToString()) / 100)));
                    lstInsptData[nIndex].Judge_Jump = string.Format("{0:f2}", Math.Abs(GetTruncateWord2Double(double.Parse(lstRawData[j + 11].ToString()) / 100)));
                    lstInsptData[nIndex].Judge_Theta = string.Format("{0:f2}", Math.Abs(GetTruncateWord2Double(double.Parse(lstRawData[j + 12].ToString()) / 100)));
                }

                nIndex++;
            }
            return true;
        }

        // 후검사 결과 Update
        public void ReadAfterInspect(int nJibNo)
        {
            // Check Connected Status
            if (m_MelsecSock.IsConnected == false)
            {
                return;
            }

            // PLC Read Start(Logging)
            LogManager.WriteLog(LogType.Information, "Get After InspectionData Start");

            // PLC Read Address
            int nPLCAddress = 0;
            if (nJibNo == 0) // Jig A
            {
                nPLCAddress = int.Parse(m_cConfig.PLC_ReadAftInspData_A);
            }
            else // Jig B
            {
                nPLCAddress = int.Parse(m_cConfig.PLC_ReadAftInspData_B);
            }

            // PLC Read
            LsPlcAftInsptDataBytes.Clear();
            byte[] tmpBytes = null;
            int nWordSize = m_cConfig.Slot * (m_cConfig.Layer / 2);

            // 2024.01.26 jh.kim 추가 - PLC read시 Retry 간격 및 횟수
            int iRetryCount = 0;
            bool bReadEnd = false;
            while (bReadEnd == false)
            {
                try
                {
		            tmpBytes = m_MelsecSock.Read(m_cConfig.PLC_Area, nPLCAddress, nWordSize);
                    for (int i = 0; i < m_cConfig.m_nRetryCount; i++) // Retry 3회
                    {
                        if (tmpBytes != null)
                        {
                            LsPlcAftInsptDataBytes.AddRange(tmpBytes);
                            bReadEnd = true;

                            break;
                        }

                        Thread.Sleep(m_cConfig.m_nRetryDelay);
                    }
                }
                catch (Exception ex)
                {
                    if (iRetryCount > m_cConfig.m_nRetryCount)
                    {
                        LogManager.WriteLog(LogType.Information, "GetInspection Data Read Error");
                        bReadEnd = true;
                    }

                    Thread.Sleep(m_cConfig.m_nRetryDelay);
                    iRetryCount++;
                }
                
            }

            // Convert Bytes to WORDS
            LsPlcAftInsptDataWORDS.Clear();
            for (int i = 0; i < LsPlcAftInsptDataBytes.ToArray().Length; i += 2)
                LsPlcAftInsptDataWORDS.Add(BitConverter.ToInt16(LsPlcAftInsptDataBytes.ToArray(), i));

            // PLC Read End(Logging)
            LogManager.WriteLog(LogType.Information, "Get After InspectionData End");

            // Buffer to GMES Data
            if (nJibNo == 0) // Jig A
            {
                ConverAfterInspectWordToGMESData(LsPlcAftInsptDataWORDS, lstGMESInsptDataAJig);
            }
            else
            {
                ConverAfterInspectWordToGMESData(LsPlcAftInsptDataWORDS, lstGMESInsptDataBJig);
            }
        }

        // 후검사 결과 WORD값을 MES용 데이터로 변환
        protected bool ConverAfterInspectWordToGMESData(List<short> lstRawData, List<InspDataGMES> lstInsptData)
        {
            // sjyi 2023.10.18 2차용접기 여부 확인 추가
            if (m_cConfig.m_bRepairWeld == false) // 1차용접기
            {
                if (lstRawData.Count == 0 || lstInsptData.Count == 0)
                {
                    return false;
                }
            }
            else // 2차용접기
            {
                // 2차용접기의 경우, 전검사가 없으므로, 리스트 초기화 필요.
                if (lstInsptData.Count == 0)
                {
                    for (int i = 0; i < m_cConfig.Slot; i++)
                    {
                        for (int j = 0; j < (m_cConfig.Layer / 2); j++)
                        {
                            InspDataGMES tmp = new InspDataGMES();
                            //22.07.03 1_1-2 -> 01_1-2 format 변경으로 인한 처리
                            tmp.Slot = (i + 1).ToString("D2");
                            switch (j)
                            {
                                case 0: tmp.Layer = "1-2"; break;
                                case 1: tmp.Layer = "3-4"; break;
                                case 2: tmp.Layer = "5-6"; break;
                                case 3: tmp.Layer = "7-8"; break;
                            }
                            lstInsptData.Add(tmp);
                        }
                    }
                }
            }

            // 후검사 데이터 만들기
            for (int i = 0; i < lstRawData.Count(); i++)
            {
                if(lstRawData[i] == (short)eJudgement.OK)
                {
                    lstInsptData[i].LsJudgement.Add(eJudgement.OK.ToString());
                }
                else if(lstRawData[i] == (short)eJudgement.NG || lstRawData[i] == (short)eJudgement.REWORK)
                {
                    lstInsptData[i].LsJudgement.Add(eJudgement.NG.ToString());
                }
            }

            return true;
        }

        public List<InspDataGMES> GetInspectData(int nJigNo)
        {
            if(nJigNo == 0)
            {
                return lstGMESInsptDataAJig;
            }
            else
            {
                return lstGMESInsptDataBJig;
            }
        }

        public bool CheckStatorModel(string strMESModelNo)
        {
            // PLC model no  1 : 77, 2 : 76, MES - MODEL INFO  11 : 76, 12 : 77
            if((strMESModelNo == "11" && GetPLCModel() == "2") || (strMESModelNo == "12" && GetPLCModel() == "1"))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        //public void SetProductInJigA(bool bIN)
        //{
        //    m_bInProductJigA = bIN;
        //}

        //public void SetProductInJigB(bool bIN)
        //{
        //    m_bInProductJigB = bIN;
        //}

        public bool GetProductInJIGA()
        {
            //return m_bInProductJigA;
			return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.A_INJIG]);
        }

        public bool GetProductInJIGB()
        {
            //return m_bInProductJigB;
			return Convert.ToBoolean(inputReportSignal[(int)INPUT_RPFRAME.B_INJIG]);
        }

        public void ClearAllFlag()
        {
            m_bInProductJigA = false;
            m_bInProductJigB = false;
        }

        // sjyi 2024.01.15, 이전장비 검사 데이터 쓰기 위해 추가
        public void WriteToPLC(int nAddress, int nWriteWords, byte[] pDatas)
        {
            if(nWriteWords > m_nMaxPLCSend)
            {
                int nAddr = nAddress;
                int nRpt = nWriteWords / m_nMaxPLCSend;
                int nByteIndex = 0;
                int nWORDSize = m_nMaxPLCSend;
                int nByteLength = nWORDSize * 2;

                byte[] pDataNew = new byte[nByteLength];

                for(int i=0; i<nRpt; i++)
                {
                    Array.Copy(pDatas, nByteIndex, pDataNew, 0, nByteLength);
                    m_MelsecSock.Write(m_cConfig.PLC_Area, nAddr, nWORDSize, pDataNew);

                    nByteIndex += nByteLength;
                    nAddr += nWORDSize;
                }

                // 나머지 Write
                nWORDSize = (nWriteWords % m_nMaxPLCSend);
                nByteLength = nWORDSize * 2;
                pDataNew = new byte[nByteLength]; // sjyi 2024.05.22 오류 슈종 
                Array.Copy(pDatas, nByteIndex, pDataNew, 0, nByteLength);
                m_MelsecSock.Write(m_cConfig.PLC_Area, nAddr, nWORDSize, pDataNew);
            }
            else
            {
                m_MelsecSock.Write(m_cConfig.PLC_Area, nAddress, nWriteWords, pDatas);
            }
        }

        public void GetSlotLayerCnt(ref int nSlot, ref int nLayer)
        {
            nSlot = m_cConfig.Slot;
            nLayer = m_cConfig.Layer;
        }

        public void Set1stResult(short nResult)
        {
            m_n1stResult = nResult;
        }

        public void CloseSocket()
        {
            if (GetIsConnected())
            {
                m_MelsecSock.Close();
            }
        }
    }
}
