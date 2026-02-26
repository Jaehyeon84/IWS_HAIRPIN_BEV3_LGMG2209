using HuVision.Model;
using HuVision.Sock;
using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HuVision
{
    public class SockManagement
    {
        public delegate void LogEventHandler(string msg);
        public event LogEventHandler LogAppended;

        public delegate void AfterInspectionEventHandler(string path, string lotID);
        public event AfterInspectionEventHandler AfterInspected;

        public event EventHandler BeforeInserted;

		// sjyi 2023.07.20 외부 사용을 위해 private 에서 public 으로 속성 변경
        public MelsecMxComponent mxComponent;
        //private MelsecSock melsecSock;

        private Thread workerThread;

        private bool isWorker = true;
        public bool isWriteAfterResult = false;
        public int totalResult = 0;

        private IniManagement iniManagement;
        public PlcModel plcModel;

        private List<byte> readPlcList = new List<byte>();
        private List<byte> writePlcList = new List<byte>();

        private bool isWait = false;

        //public List<BefInspModel> befInspModelList = new List<BefInspModel>();

        public SockManagement()
        {
            iniManagement = IniManagement.Instance();
            plcModel = new PlcModel();

            //// 초기화
            //for (int i = 1; i < iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots + 1; i++)
            //{
            //    for (int j = 0; j < (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2); j++)
            //    {
            //        BefInspModel model = new BefInspModel();
            //        model.slot = i;
            //        switch (j)
            //        {
            //            case 0: model.layer = "1-2"; break;
            //            case 1: model.layer = "3-4"; break;
            //            case 2: model.layer = "5-6"; break;
            //            case 3: model.layer = "7-8"; break;
            //            case 4: model.layer = "9-10"; break;
            //            case 5: model.layer = "11-12"; break;
            //        }

            //        befInspModelList.Add(model);
            //    }
            //}
        }

        public void Open()
        {
            mxComponent = new MelsecMxComponent();
            mxComponent.Init(int.Parse(iniManagement.iniModel.PLC_IP));

            //melsecSock = new MelsecSock(iniManagement.iniModel.PLC_IP, iniManagement.iniModel.PLC_Port);
            //melsecSock.StartConnectThread();
            
            workerThread = new Thread(new ThreadStart(Worker));
            workerThread.Start();
        }

        public void Close()
        {
            isWorker = false;

            try
            {
                if (workerThread != null)
                {
                    workerThread.Abort();
                    workerThread = null;
                }
            }
            catch { }

            if (mxComponent != null)
            {
                mxComponent.Exit();
                mxComponent = null;
            }

            //if (melsecSock != null)
            //{
            //    melsecSock.Exit();
            //    melsecSock = null;
            //}
        }

        private void Worker()
        {
            WriteMxConponent(iniManagement.iniModel.PLC_WriteStatus, new byte[] { 0, 0 });
            WriteMxConponent(iniManagement.iniModel.PLC_WriteAfterResult, new byte[] { 0, 0 });

            while (isWorker)
            {
                try
                {
                    if (iniManagement.iniModel.Process_Manual)
                        continue;

                    if (!plcModel.SetReadStatus(mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_ReadStatus.ToString(), 30)))      // PLC 읽기 영역 읽어오기
                        continue;

                    if (!plcModel.SetWriteStatus(mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_WriteStatus.ToString(), 10)))    // PLC 쓰기 영역 읽어오기
                        continue;
                                                            
                    if (isWait)
                        if (plcModel.CheckInit())
                            isWait = false;

                    if (plcModel.CompareInputPLCValue())    // PLC 영역 값이 바뀌었을 때 경우만 로그 출력
                    {
                        OnAppendLog(string.Format("Input Signal Change: Insp: {0}, PlcModel: {1}, Rcp: {2}",
                            plcModel.inputSignal[(int)INPUT_FRAME.INSP], plcModel.inputSignal[(int)INPUT_FRAME.PLC_MODEL], plcModel.inputSignal[(int)INPUT_FRAME.RCP]));

                        if (plcModel.inputSignal[(int)INPUT_FRAME.INSP] == 0)  // Insp End1
                        {
                            plcModel.outputSignal[(int)OUTPUT_FRAME.INSP] = 0;

                            WriteMxConponent(iniManagement.iniModel.PLC_WriteStatus, new byte[] { 0, 0 });
                            WriteMxConponent(iniManagement.iniModel.PLC_WriteAfterResult, new byte[] { 0, 0 });
                        }
                        else if (plcModel.inputSignal[(int)INPUT_FRAME.INSP] == 1) // Insp Start
                        {
                            if(!isWait)
                            {
                                isWait = true;
                                OnAppendLog("After Insp End");

                                // 후검사 파일 경로 읽기(60 Words)
                                // byte[] data = mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_ReadAfterPath.ToString(), 150);
                                byte[] data = mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_ReadAfterPath.ToString(), 100);
                                // string path = Encoding.ASCII.GetString(data).Trim('\0').TrimEnd();
                                string path = Encoding.ASCII.GetString(data);

                                OnAppendLog(string.Format("[PLC Read] {0}\n", path));

                                // 후검사 파일 경로에 Null문자 및 기타 Noise성 데이터가 포함되어 있어, 문자열 정리하는 부분 추가
                                //char[] charsToTrim = { '\t', '\0', ',', '.', '|' };
                                //string[] words = path.Split();
                                //path = path.Replace("\0", " ");
                                int nFind = path.IndexOf(".Bin");
                                if(nFind >= 0)
                                {
                                    path = path.Remove(nFind + 4);
                                }

                                //                                path = path.TrimEnd(charsToTrim);
                                OnAppendLog(string.Format("[Replace] {0}\n", path));

                                if (AfterInspected != null)
                                    AfterInspected(path, plcModel.lotIdBuild.ToString().Trim('\0').TrimEnd());
                                //AfterInspected(analysisList[(int)ANALYSIS.DATA].Split(';')[5].Replace(@"//", @"/"), plcModel.lotIdBuild.ToString());
                            }
                           
                        }
                    }
                 
                    if (plcModel.CompareOutputPLCValue())
                    {
                        OnAppendLog(string.Format("Output Signal Change: Insp: {0}",
                           plcModel.inputSignal[(int)OUTPUT_FRAME.INSP]));
                    }

                    // 용접 후 검사 결과 Write
                    if(isWriteAfterResult)
                    {
                        bool isResult = WriteMxConponent(iniManagement.iniModel.PLC_WriteReWorkData, writePlcList.ToArray());// WriteAfterInspResult(iniManagement.iniModel.PLC_WriteReWorkData);
                        if(!isResult)
                            OnAppendLog("WriteAfterInspResult Error");

                        OnAppendLog(string.Format("Result: {0}, TotalResult: {1}", isResult, totalResult));

                        ResultWriteToPLC(iniManagement.iniModel.PLC_WriteAfterResult, !isResult, totalResult);
                        isWriteAfterResult = false;

                        plcModel.outputSignal[(int)OUTPUT_FRAME.INSP] = 1;

                        OnAppendLog("End.");
                    }

                    WriteMxConponent(iniManagement.iniModel.PLC_WriteStatus, plcModel.GetOutputStatus()); // 상태정보 Write
                }
                catch (Exception e)
                {
                    OnAppendLog("Worker: " + e.Message);
                }
                finally
                {
                    plcModel.ChangeInputSignal();                    
                    Thread.Sleep(250);
                }
            }
        }       


        /// <summary>
        /// 전 검사 및 후 검사 완료 
        /// </summary>
        //private bool ProcessingStatus()
        //{
        //    if (isWait)
        //        return false;

        //    //// 용접 전 검사 완료
        //    //if (plcModel.CheckBeforeEndInspection())
        //    //{
        //    //    isWait = true;
        //    //    OnAppendLog("Before Insp End");

        //    //    SaveLastPosition();

        //    //    if (BeforeInserted != null)
        //    //        BeforeInserted(null, null);

        //    //    return true;
        //    //}

        //    // 용접 후 검사 완료
        //    if (plcModel.CheckAfterEndInspection())
        //    {
        //        isWait = true;
        //        OnAppendLog("After Insp End");

        //        // 후검사 파일 경로 읽기
        //        byte[] data = mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_ReadAfterPath.ToString(), 50);
        //        string path = Encoding.Default.GetString(data).TrimEnd('\0').TrimEnd();

        //        if (AfterInspected != null)
        //            AfterInspected(path, plcModel.lotIdBuild.ToString());
        //            //AfterInspected(analysisList[(int)ANALYSIS.DATA].Split(';')[5].Replace(@"//", @"/"), plcModel.lotIdBuild.ToString());
        //    }

        //    return false;
        //}

        //public void SaveLastPosition()
        //{
        //    string path = Application.StartupPath + "\\lastPst.dat";
        //    using (StreamWriter sw = new StreamWriter(path, false))
        //    {
        //        byte[] befData = GetBefInspectionData();
        //        readPlcList.Clear();
                
        //        if(befData == null)
        //        {
        //            OnAppendLog("SaveLastPosition BefData null");
        //            return;
        //        }

                
        //        OnAppendLog("BefInspModel Rcp: " + plcModel.inputSignal[(int)INPUT_FRAME.RCP]);

        //        int slot, layer;
        //        if (plcModel.inputSignal[(int)INPUT_FRAME.RCP] < 3) // 1 = 77 A, 2 = 77 B, 3 = 76 A, 4 = 77 B
        //        {
        //            slot = iniManagement.iniModel.Model77_Slot;
        //            layer = iniManagement.iniModel.Model77_Layer;
        //        }
        //        else
        //        {
        //            slot = iniManagement.iniModel.Model76_Slot;
        //            layer = iniManagement.iniModel.Model76_Layer;
        //        }

        //        befInspModelList.Clear();
        //        for (int i = 1; i < slot + 1; i++)
        //        {
        //            for (int j = 0; j < (layer / 2); j++)
        //            {
        //                BefInspModel model = new BefInspModel();
        //                model.slot = i;
        //                switch (j)
        //                {
        //                    case 0: model.layer = "1-2"; break;
        //                    case 1: model.layer = "3-4"; break;
        //                    case 2: model.layer = "5-6"; break;
        //                    case 3: model.layer = "7-8"; break;
        //                    case 4: model.layer = "9-10"; break;
        //                    case 5: model.layer = "11-12"; break;
        //                }

        //                befInspModelList.Add(model);
        //            }
        //        }


        //        sw.WriteLine("Slot,Layer,Result,1H,2H,3H,1A,2A,AX,AY,BX,BY,C,Jump,Theta,Tmp1,Tmp2,Tmp3");

        //        int idx = 0;
        //        for (int i = 0; i < befData.Length; i += 32 )
        //        {
        //            befInspModelList[idx].result = BitConverter.ToInt16(befData, i);

        //            sw.WriteLine("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17}", 
        //               befInspModelList[idx].slot,                                      // Slot
        //               befInspModelList[idx].layer,                                     // Layer
        //               befInspModelList[idx].result / 100,                                    // Result   
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 2) / 100),      // 1H
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 4) / 100),      // 2H
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 6) / 100),      // 3H
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 8) / 100),      // 1A
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 10) / 100),     // 2A
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 12) / 100),     // aX
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 14) / 100),     // bY
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 16) / 100),     // bX
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 18) / 100),     // bY
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 20) / 100),     // C
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 22) / 100),     // Jump
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 24) / 100),     // Theta
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 26) / 100),     // tmp1
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 28) / 100),     // tmp2
        //               UserUtil.GetTruncate(BitConverter.ToInt16(befData, i + 30) / 100)      // tmp3
        //               );
        //            idx++;
        //        }
        //    }     
        //}

        //private void AppendWritePlcList(List<string> valueList, bool isTt = false)
        //{
        //    foreach (string value in valueList)
        //    {
        //        //tmpValue = (Int16)(double.Parse(value) * 100);
        //        if(isTt)
        //        {
        //            UInt16 tmpValue = UInt16.Parse(value);// (Int16)(double.Parse(value));
        //            writePlcList.AddRange(BitConverter.GetBytes(tmpValue));
        //        }
        //        else
        //        {
        //            Int16 tmpValue = Int16.Parse(value);
        //            writePlcList.AddRange(BitConverter.GetBytes(tmpValue));
        //        }
        //    }
        //}

        private void OnDataLog(string command, string subCommand, string log)
        {
            OnAppendLog("Recv Data Null Command: " + command + " SubCommand: " + subCommand);
        }

        private void ResultWriteToPLC(string addr, bool isAlarm, int totalResult = (short)RESPONSE.OK)
        {
            short tmpResult = (short)RESPONSE.OK;
            string sMsg = "PLC Write - Pass";
            string fMsg = "PLC Write Fail - Pass";

            if (isAlarm || totalResult == (int)RESPONSE.ALARM)
            {
                tmpResult = (short)RESPONSE.ALARM;
                sMsg = "PLC Write - Alram";
                fMsg = "PLC Write Fail - Alarm";
            }
            else if (totalResult == (int)RESPONSE.NG)
            {
                tmpResult = (short)RESPONSE.NG;
                sMsg = "PLC Write - Rework";
                fMsg = "PLC Write Fail - Rework";
            }

            WriteMxConponent(addr, BitConverter.GetBytes(tmpResult), sMsg, fMsg);
            OnAppendLog("PLC Write End");
        }

		// sjyi 2023.07.20 외부 사용을 위해 private 에서 public 으로 속성 변경
        public void OnAppendLog(string msg)
        {
            if (LogAppended != null)
                LogAppended(msg);
        }

        //private void ResultSignalInit()
        //{
        //    WriteMxConponent(iniManagement.iniModel.PLC_WriteBeforeResult, BitConverter.GetBytes((short)RESPONSE.NONE));
        //    WriteMxConponent(iniManagement.iniModel.PLC_WriteAfterResult, BitConverter.GetBytes((short)RESPONSE.NONE));

        //    OnAppendLog("Result Signal Initialize");
        //}

        //public bool WriteMelsec(int addr, byte[] data, string sMsg = "", string fMsg = "")
        //{
        //    if (!mxComponent.Write(iniManagement.iniModel.PLC_Area, addr.ToString(), data, "B"))
        //    {
        //        if (!string.IsNullOrWhiteSpace(fMsg))
        //            OnAppendLog(fMsg);

        //        return false;
        //    }

        //    if (!string.IsNullOrWhiteSpace(sMsg))
        //        OnAppendLog(sMsg);

        //    return true;
        //}

        public void SetWritePlcList(byte[] data)
        {
            writePlcList.Clear();
            writePlcList.AddRange(data);
        }

        //private bool WriteAfterInspResult(int addr)
        //{
        //    try
        //    {
        //        // PLC Write Result Data
        //        int partWord = 300;
        //        int totalWord = writePlcList.Count / 2;
        //        int loopCount = (int)Math.Ceiling(totalWord / (double)partWord);
        //        int tmpWord = 0;

        //        int startDataPst = 0;

        //        byte[] tmpBytes = null;
        //        for (int i = 0; i < loopCount; i++)
        //        {
        //            tmpWord = Math.Min(totalWord, partWord);
        //            tmpBytes = writePlcList.GetRange(startDataPst, tmpWord * 2).ToArray();

        //            if (!WriteMelsec(addr, tmpBytes))
        //            {
        //                OnAppendLog("WriteAfterInspResult: WriteMelsec Fail");
        //                return false;
        //            }

        //            totalWord -= tmpWord;
        //            addr += tmpWord;
        //        }

        //        return true;

        //    }
        //    catch (Exception e)
        //    {
        //        OnAppendLog("WriteAfterInspResult: " + e.Message);
        //        return false;
        //    }
        //}

        public bool WriteMxConponent(string addr, byte[] data, string sMsg = "", string fMsg = "")
        {
            if (!mxComponent.Write(iniManagement.iniModel.PLC_Area, addr, data, "B"))
            {
                if (!string.IsNullOrWhiteSpace(fMsg))
                    OnAppendLog(fMsg);

                return false;
            }

            if (!string.IsNullOrWhiteSpace(sMsg))
                OnAppendLog(sMsg);

            return true;
        }

        /// <summary>
        /// 용접 전 검사 데이터 읽기
        /// </summary>
        /// <returns></returns>
        //private byte[] GetBefInspectionData()
        //{
        //    try
        //    {
        //        //readPlcList.Clear();

        //        //int addr = int.Parse(iniManagement.iniModel.PLC_ReadInspectionData);
        //        //int partWord = 1000;
        //        //int totalWord = (96 * 16) * (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2);
        //        //int loopCount = (int)Math.Ceiling(totalWord / (double)partWord);
        //        //int tmpWord = totalWord;

        //        //byte[] tmpBytes = null;
        //        //for (int i = 0; i < loopCount; i++)
        //        //{
        //        //    tmpWord = Math.Min(totalWord, partWord);
        //        //    tmpBytes = mxComponent.Read(iniManagement.iniModel.PLC_Area, addr.ToString(), tmpWord);
        //        //    if (tmpBytes == null)
        //        //    {
        //        //        OnAppendLog("GetBefInspectionData: Read Fail Inspection Before Data");
        //        //        return null;
        //        //    }

        //        //    readPlcList.AddRange(tmpBytes);
        //        //    totalWord -= tmpWord;
        //        //    addr += tmpWord;
        //        //}

        //        //return readPlcList.ToArray();

        //        int totalWord = (96 * 16) * (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2);
        //        byte[] recvBefData = mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_ReadInspectionData, totalWord, "B");
        //        if (recvBefData == null)
        //            OnAppendLog("GetBefInspectionData: Read Fail Inspection Before Data");

        //        return recvBefData;
        //    }
        //    catch(Exception e)
        //    {
        //        OnAppendLog("GetBefInspectionData: " + e.Message);
        //        return null;
        //    }

        //}

        //public void SockTest()
        //{
        //    List<string> valueList = new List<string>();
        //    string[] tokken = "275.31;275.51;275.39;275.43;282.74;282.8;282.84;282.89;289.8;289.86;289.92;290.23;297.22;297.57;297.36;297.41;305.08;305.16;305.23;305.29;312.78;312.19;312.75;312.33;319.6;319.69;319.77;319.84;327.08;327.18;327.26;327.33;335.5;334.72;334.8;334.87;342.09;342.2;342.8;342.36;349.67;349.77;349.85;350.4;357.18;357.28;357.36;357.44;4.79;4.88;5.39;5.03;12.37;12.47;12.81;12.87;20.02;20.1;20.17;20.23;28.01;28.06;28.14;28.19;35.27;35.33;35.39;35.48;43.2;42.53;42.57;42.62;50.86;50.61;50.65;50.67;58.19;58.22;58.45;58.27;65.89;65.91;65.78;65.94;73.15;73.15;73.16;73.17;80.45;80.44;80.87;80.73;88.48;88.52;88.45;88.43;276.11;275.66;275.64;275.63;283.37;283.34;283.31;283.28;290.98;290.93;290.89;290.86;298.64;298.69;298.54;298.5;306.65;306.59;306.53;306.02;313.48;313.42;313.36;313.31;321.04;320.96;321.11;320.85;329.03;328.95;328.89;328.84;336.49;336.07;336;335.94;343.72;343.64;343.57;343.64;351.19;351.11;351.05;351.01;358.75;358.5;358.58;358.52;6.2;6.12;6.06;6.01;13.3;13.23;13.18;13.48;20.75;20.69;20.64;20.59;28.15;28.49;28.48;28.41;35.21;35.16;35.13;35.09;43.59;42.55;42.52;42.5;50.4;50.38;50.36;50.35;58.38;57.79;57.78;57.77;65.22;65.63;65.22;65.22;72.61;72.62;72.63;73.06;80;80.02;80.04;80.05;86.87;86.9;87.87;87.81".Split(';');

        //    writePlcList.Clear();

        //    foreach (string str in tokken)
        //        valueList.Add(str);

        //    Int16 tmpValue = 0;
        //    foreach (string value in valueList)
        //    {
        //        tmpValue = (Int16)(double.Parse(value) * 100);
        //        writePlcList.AddRange(BitConverter.GetBytes(tmpValue));
        //    }

        //    if (!WriteMxConponent(iniManagement.iniModel.PLC_WriteHeight, null, "PLC Write - Height", "PLC Write Fail - Height"))
        //    {
        //        ResultWriteToPLC(iniManagement.iniModel.PLC_WriteBeforeResult, true);
        //        return;
        //    }

        //}

		// sjyi 2023.07.20 재검사 핀 데이터 가져오기 위한 함수 추가(2차용접기용)
        public byte[] GetPLCResultData()
        {
            OnAppendLog(string.Format("Read ReworkData PLC Addr : {0}", iniManagement.iniModel.PLC_ReadReWorkData.ToString()));
            try
            {
                byte[] ReadPLCResultData = mxComponent.Read(iniManagement.iniModel.PLC_Area, iniManagement.iniModel.PLC_ReadReWorkData.ToString(), 384);
                return ReadPLCResultData;
            }
            catch
            {
                return null;
            }
        }
    }
}
