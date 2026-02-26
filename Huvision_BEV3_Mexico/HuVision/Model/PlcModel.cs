using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HuVision.Model
{
    public class PlcModel
    {
        public short[] inputSignal = new short[4];    // LINK, INSP
        public short[] befInputSignal = new short[4]; // LINK, INSP
        public short[] outputSignal = new short[3];    // LINK, INSP, RESULT
        public short[] befOutputSignal = new short[3];    // LINK, INSP, RESULT

        public short SignalInsptMode;
                
        private BitArray bitArray;
        //private ushort tmpBit;

        //private List<byte> writePlcList = new List<byte>();

        public StringBuilder lotIdBuild = new StringBuilder();

        private TimeSpan span;
        private DateTime lastTime;

        public PlcModel()
        {
            lastTime = DateTime.Now;
        }

        /// <summary>
        /// 읽기 영역 값 세팅
        /// </summary>
        public bool SetReadStatus(byte[] recvData)
        {
            if (recvData == null)
                return false;

            if (recvData.Length != 60)
                return false;

            bitArray = new BitArray(new byte[] { recvData[0], recvData[1] });

            inputSignal[(int)INPUT_FRAME.LINK] = GetBitToUshort(bitArray[0]);  
            inputSignal[(int)INPUT_FRAME.INSP] = GetBitToUshort(bitArray[1]);

            inputSignal[(int)INPUT_FRAME.PLC_MODEL] = BitConverter.ToInt16(recvData, 22);
//            inputSignal[(int)INPUT_FRAME.RCP] = BitConverter.ToInt16(recvData, 18); 
            inputSignal[(int)INPUT_FRAME.RCP] = BitConverter.ToInt16(recvData, 26); 

            lotIdBuild.Clear();
//            lotIdBuild.Append(Encoding.Default.GetString(recvData.Skip(34).Take(26).ToArray()).TrimEnd('\0'));
//            lotIdBuild.Append(Encoding.Default.GetString(recvData.Skip(32).Take(26).ToArray()).Trim('\0').Replace('\0', ' '));
             lotIdBuild.Append(Encoding.Default.GetString(recvData.Skip(32).Take(26).ToArray()).Trim('\0'));
           // string path = Encoding.Unicode.GetString(data).Trim('\0').TrimEnd();

            return true;
        }

        /// <summary>
        /// 쓰기 영역 값 세팅
        /// </summary>
        public bool SetWriteStatus(byte[] recvData)
        {
            if (recvData == null)
                return false;

            bitArray = new BitArray(new byte[] { recvData[0], recvData[1] });

            outputSignal[(int)OUTPUT_FRAME.LINK] = GetBitToUshort(bitArray[0]);   
            outputSignal[(int)OUTPUT_FRAME.INSP] = GetBitToUshort(bitArray[1]);    

            //outputSignal[(int)OUTPUT_FRAME.AFT_RESULT] = BitConverter.ToInt16(recvData, 8);            // 용접 검사 후 결과 전송 완료 (0 = None, 1=Pass, 2=Rework, 3=Alarm)

            return true;
        }

        /// <summary>
        /// PLC에서 검사모드 읽어오는 함수(PLC가 Write하는 영역)
        /// </summary>
        /// <param name="recvData">[1] = 전 검사, [2] = 후 검사</param>
        /// <returns></returns>
        public bool SetReadInsptMode(byte[] recvData)
        {
            if (recvData == null)
                return false;

            if (recvData.Length != 2)
                return false;

            SignalInsptMode = BitConverter.ToInt16(recvData, 0);
            //LogManager.WriteLog(LogType.Information, "InsptMode = " + SignalInsptMode);

            return true;
        }
               
        /// <summary>
        /// Input Signal을 전에 받았 던 데이터와 비교
        /// </summary>
        public bool CompareInputPLCValue()
        {
            if (inputSignal[(int)INPUT_FRAME.INSP] != befInputSignal[(int)INPUT_FRAME.INSP])
                return true;

            if (inputSignal[(int)INPUT_FRAME.PLC_MODEL] != befInputSignal[(int)INPUT_FRAME.PLC_MODEL])
                return true;

            if (inputSignal[(int)INPUT_FRAME.RCP] != befInputSignal[(int)INPUT_FRAME.RCP])
                return true;

            return false;
        }

        public bool CompareOutputPLCValue()
        {
            if (outputSignal[(int)OUTPUT_FRAME.INSP] != befOutputSignal[(int)OUTPUT_FRAME.INSP])
                return true;

            //if (outputSignal[(int)OUTPUT_FRAME.AFT_RESULT] != befOutputSignal[(int)OUTPUT_FRAME.AFT_RESULT])
            //    return true;

            return false;
        }
                                       
        /// <summary>
        /// InputSignal 보관
        /// </summary>
        public void ChangeInputSignal()
        {
           
            befInputSignal[(int)INPUT_FRAME.INSP] = inputSignal[(int)INPUT_FRAME.INSP];
            befInputSignal[(int)INPUT_FRAME.PLC_MODEL] = inputSignal[(int)INPUT_FRAME.PLC_MODEL];
            befInputSignal[(int)INPUT_FRAME.RCP] = inputSignal[(int)INPUT_FRAME.RCP];

            befOutputSignal[(int)OUTPUT_FRAME.INSP] = outputSignal[(int)OUTPUT_FRAME.INSP];
            //befOutputSignal[(int)OUTPUT_FRAME.AFT_RESULT] = outputSignal[(int)OUTPUT_FRAME.AFT_RESULT];
        }
                
        ///// <summary>
        ///// CMES에서 받은 데이터 output에 써주기
        ///// </summary>
        //public void SetOutputStatus(string[] tokken)
        //{
        //    outputSignal[(int)OUTPUT_FRAME.START] = Int16.Parse(tokken[0]);
        //    outputSignal[(int)OUTPUT_FRAME.END] = Int16.Parse(tokken[1]);
        //    outputSignal[(int)OUTPUT_FRAME.INIT] = Int16.Parse(tokken[2]);
        //    outputSignal[(int)OUTPUT_FRAME.TIME] = Int16.Parse(tokken[3]);
        //    outputSignal[(int)OUTPUT_FRAME.TOTAL] = Int16.Parse(tokken[4]);

        //    //// 완료응답 이전에 결과값이 들어오면 0으로 초기화
        //    if (outputSignal[(int)OUTPUT_FRAME.END] == 0 && outputSignal[(int)OUTPUT_FRAME.TOTAL] != 0)
        //        outputSignal[(int)OUTPUT_FRAME.TOTAL] = 0;
        //}

        ///// <summary>
        ///// 수동 테스트 시 사용
        ///// </summary>
        //public void SetManualValue(short start, short end, string mode)
        //{
        //    inputSignal[(int)INPUT_FRAME.START] = start;
        //    inputSignal[(int)INPUT_FRAME.END] = end;
        //    inputSignal[(int)INPUT_FRAME.INIT] = 0;
        //    inputSignal[(int)INPUT_FRAME.TIME] = 0;
        //    inputSignal[(int)INPUT_FRAME.LINK] = 0;

        //    inputSignal[(int)INPUT_FRAME.MODE] = mode == "1" ? (short)1 : (short)2;
        //    //inputSignal[(int)INPUT_FRAME.TYPE] = 1;
        //    inputSignal[(int)INPUT_FRAME.RESPONSE] = 1;

        //    DateTime nowTime = DateTime.Now;
        //    inputSignal[(int)INPUT_FRAME.YYYY] = short.Parse(nowTime.ToString("yyyy"));
        //    inputSignal[(int)INPUT_FRAME.MM] = short.Parse(nowTime.ToString("MM"));
        //    inputSignal[(int)INPUT_FRAME.DD] = short.Parse(nowTime.ToString("dd"));
        //    inputSignal[(int)INPUT_FRAME.HH] = short.Parse(nowTime.ToString("HH"));
        //    inputSignal[(int)INPUT_FRAME.mm] = short.Parse(nowTime.ToString("mm"));
        //    inputSignal[(int)INPUT_FRAME.ss] = short.Parse(nowTime.ToString("ss"));
        //}

        /// <summary>
        /// output 상태를 PLC에 쓰기 위한 데이터 변환
        /// </summary>
        public byte[] GetOutputStatus()
        {
            //writePlcList.Clear();
            
            span = DateTime.Now - lastTime;            
            if (span.TotalSeconds >= 1)
            {
                outputSignal[(int)OUTPUT_FRAME.LINK] = (short)(outputSignal[(int)OUTPUT_FRAME.LINK] == 1 ? 0 : 1);
                lastTime = DateTime.Now;
            }

            ushort tmpBit = Convert.ToUInt16(string.Format("00000000000000{1}{0}",
               outputSignal[(int)OUTPUT_FRAME.LINK],
               outputSignal[(int)OUTPUT_FRAME.INSP]), 2);

            
            return BitConverter.GetBytes(tmpBit);
        }

        private short GetBitToUshort(bool value)
        {
            if (value == true)
                return (short)1;
            else
                return (short)0;
        }


        ///// <summary>
        ///// 용접 전 검사 완료 체크
        ///// CMES에서 완료 처리가 되었을 경우
        ///// </summary>
        //public bool CheckBeforeEndInspection()
        //{
        //    // 검사모드가 용접 전 검사이면서, 완료 신호 수신 시 용접 후 검사 처리
        //    if (inputSignal[(int)INPUT_FRAME.MODE] == 1 && outputSignal[(int)OUTPUT_FRAME.END] == 1)
        //        return true;

        //    return false;
        //}

        ///// <summary>
        ///// 용접 후 검사 체크
        ///// CMES에서 완료 처리가 되었을 경우
        ///// </summary>
        //public bool CheckAfterEndInspection()
        //{
        //    // 검사모드가 용접 후 검사이면서, 완료 신호 수신 시 용접 후 검사 처리
        //    if (inputSignal[(int)INPUT_FRAME.MODE] == 2 && outputSignal[(int)OUTPUT_FRAME.END] == 1)
        //        return true;

        //    return false;
        //}

        ///// <summary>
        ///// 시작 & 종료 신호 초기화 체크
        ///// </summary>
        ///// <returns></returns>
        //public bool CheckInitSignal()
        //{
        //    if (inputSignal[(int)INPUT_FRAME.START] == 0 && inputSignal[(int)INPUT_FRAME.END] == 0 && 
        //        outputSignal[(int)OUTPUT_FRAME.START] == 0 && outputSignal[(int)OUTPUT_FRAME.END] == 0)
        //        return true;

        //    return false;
        //}

        public bool CheckInit()
        {
            if (inputSignal[(int)INPUT_FRAME.INSP] == 0 && inputSignal[(int)OUTPUT_FRAME.INSP] == 0)
                return true;

            return false;
        }
    }
}
