using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HuVision.Model;
using System.Drawing.Drawing2D;
using System.IO;
using DevExpress.XtraGrid.Views.Grid;
using DevExpress.XtraCharts;
using System.Diagnostics;
using System.Threading;

namespace HuVision.UI
{
    public partial class UcInspection : UserControl
    {
        //private DataTable dataTable = new DataTable();
        //private DataTable ngDataTable = new DataTable();

        // 0 = Total, 1 = Ng
        private DataTable[] tableArr = new DataTable[2];
        private GridView[] gridViewArr = new GridView[2];

        private IniManagement iniManagement;

        private double D2R = 0.0174532925199433;

        //public List<short> beforeInspectionData = new List<short>();   // 용접 전 검사 데이터
        //private short[] beforeInspectionArr;    

        private short[] inspectionResultArr = null;    // 용접 후 검사 결과
        private short[] drawResultArr = null;          // Slot Drawing 데이터 

        private Font drawFont = new Font("Consolas", 8);
        private SolidBrush okBrush = new SolidBrush(Color.White);
        private SolidBrush ngBrush = new SolidBrush(Color.Red);

        private DataTable radialTable = new DataTable();
        private DataTable tangentialTable = new DataTable();

        //private List<float[]> radialChartList = new List<float[]>();
        //private List<float[]> tangentialChartList = new List<float[]>();

        public int totalResult = 0;

        private string befLotID;
        private int productCount;

        private Dictionary<string, int> aftPointDict = new Dictionary<string, int>();
        private string aftErrorPoint = string.Empty;

        public int okCount = 0, reworkCount = 0, failCount = 0;
        public string saveCsvPath = string.Empty;
        public string saveImgPath = string.Empty;

        private enum COL { Slot = 0, Layer, Result, Col1, Col2, Col3, Col4, Col5, Col6, Col7, Col8, Col9, Col10, Radial, Tangential };

        // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
        public int m_rcp;
        private int m_slot, m_layer;

        // sjyi 2023.07.20  재용접 포인트 개수 확인용 변수 추가 
        public int m_nReworkCnt;

        // sjyi 2024.05.22 재용접 포인트 저장용.
        public List<BefInspModel> befInspModelList = new List<BefInspModel>();

        public UcInspection()
        {
            InitializeComponent();

            iniManagement = IniManagement.Instance();

            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);

            // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
            m_slot = iniManagement.iniModel.Model77_Slot;
            m_layer = iniManagement.iniModel.Model77_Layer;

            for (int i = 0; i < 2; i++)
                tableArr[i] = new DataTable();

            gridControl.DataSource = tableArr[0];
            gridControl_NG.DataSource = tableArr[1];

            gridViewArr[0] = gridView;
            gridViewArr[1] = gridView_NG;

            InitGridControl();
            //InitChart(radialTable, cc_Radial, "Radial");
            //InitChart(tangentialTable, cc_Tangential, "Tangential");

            //if (File.Exists(Application.StartupPath + "\\type.png"))
            //    pb_Type.Image = Image.FromFile(Application.StartupPath + "\\type.png");

            // sjyi 2023.07.20  재용접 포인트 개수 확인용 변수 추가
            m_nReworkCnt = 0;
        }

        private void UcInspection_Load(object sender, EventArgs e)
        {
            //if(rcp == 0)
            //{
            //    inspectionResultArr = new short[iniManagement.iniModel.Model77_Slot * (iniManagement.iniModel.Model77_Layer / 2)];
            //    for (int i = 0; i < inspectionResultArr.Length; i++)
            //        inspectionResultArr[i] = (int)RESPONSE.OK;

            //    drawResultArr = new short[iniManagement.iniModel.Model77_Slot];
            //    for (int i = 0; i < drawResultArr.Length; i++)
            //        drawResultArr[i] = (int)RESPONSE.OK;
            //}
            //else
            //{
            //    inspectionResultArr = new short[slot * (layer / 2)];
            //    for (int i = 0; i < inspectionResultArr.Length; i++)
            //        inspectionResultArr[i] = (int)RESPONSE.OK;

            //    drawResultArr = new short[slot];
            //    for (int i = 0; i < drawResultArr.Length; i++)
            //        drawResultArr[i] = (int)RESPONSE.OK;
            //}
        }

        private void InitGridControl()
        {
            for (int i = 0; i < 2; i++)
            {
                CreateColumn("Slot", "Slot", 30, tableArr[i], gridViewArr[i], typeof(int));
                CreateColumn("Pin", "Pin", 30, tableArr[i], gridViewArr[i], typeof(string));
                CreateColumn("Result", "Result", 55, tableArr[i], gridViewArr[i], typeof(int));                    // Result

                SetColumn(iniManagement.iniModel.Coulmn_Col1, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col2, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col3, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col4, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col5, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col6, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col7, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col8, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col9, tableArr[i], gridViewArr[i]);
                SetColumn(iniManagement.iniModel.Coulmn_Col10, tableArr[i], gridViewArr[i]);
            }


            //for (int i = 1; i < iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots + 1; i++)
            //{
            //    for (int j = 0; j < (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2); j++)
            //    {
            //        DataRow row = tableArr[0].NewRow();

            //        row[(int)COL.Slot] = i;
            //        switch (j)
            //        {
            //            case 0: row[(int)COL.Layer] = "1-2"; break;
            //            case 1: row[(int)COL.Layer] = "3-4"; break;
            //            case 2: row[(int)COL.Layer] = "5-6"; break;
            //            case 3: row[(int)COL.Layer] = "7-8"; break;
            //            case 4: row[(int)COL.Layer] = "9-10"; break;
            //            case 5: row[(int)COL.Layer] = "11-12"; break;
            //        }

            //        aftPointDict.Add(i.ToString() + " " + row[(int)COL.Layer].ToString(), (int)RESPONSE.OK);

            //        for (int k = (int)COL.Result; k < (int)COL.Col10 + 1; k++)
            //            row[k] = 0.0;

            //        row[(int)COL.Result] = 1;

            //        tableArr[0].Rows.Add(row);
            //    }
            //}
        }

        private void SetColumn(string value, DataTable table, GridView tmpGridView)
        {
            string[] tokken = value.Split(',');
            CreateColumn(tokken[0], tokken[0], int.Parse(tokken[1]), table, tmpGridView, typeof(string));
        }

        public void SetRcp(int rcp)
        {
            // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
            this.m_rcp = rcp;
            if (rcp < 3)
            {
                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                m_slot = iniManagement.iniModel.Model77_Slot;
                m_layer = iniManagement.iniModel.Model77_Layer;
            }
            else
            {
                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                m_slot = iniManagement.iniModel.Model76_Slot;
                m_layer = iniManagement.iniModel.Model76_Layer;
            }


        }

        public void SetModel(int nModel)
        {
            if (nModel == 1)
            {
                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                m_slot = iniManagement.iniModel.Model77_Slot;
                m_layer = iniManagement.iniModel.Model77_Layer;
            }
            else if (nModel == 2)
            {
                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                m_slot = iniManagement.iniModel.Model76_Slot;
                m_layer = iniManagement.iniModel.Model76_Layer;
            }
            else
            {
                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                m_slot = iniManagement.iniModel.Model77_Slot;
                m_layer = iniManagement.iniModel.Model77_Layer;
            }
        }

        //private void InitChart(DataTable dataTable, ChartControl chartControl, string name)
        //{
        //    dataTable.Columns.Add(name, typeof(string));
        //    dataTable.Columns.Add("X", typeof(float));
        //    dataTable.Columns.Add("Y", typeof(float));

        //    chartControl.Series.Clear();
        //    chartControl.SeriesDataMember = name;
        //    chartControl.SeriesTemplate.ChangeView(ViewType.Line);
        //    chartControl.SeriesTemplate.ArgumentDataMember = "X";
        //    chartControl.SeriesTemplate.ValueDataMembers.AddRange(new string[] { "Y" });

        //    chartControl.DataSource = dataTable;


        //    XYDiagram diagram = (XYDiagram)chartControl.Diagram;
        //    diagram.AxisX.WholeRange.Auto = false;
        //    diagram.AxisX.WholeRange.MinValue = 0;
        //    diagram.AxisX.WholeRange.MaxValue = 150;

        //    diagram.AxisY.WholeRange.Auto = false;
        //    diagram.AxisY.WholeRange.MinValue = 0;
        //    diagram.AxisY.WholeRange.MaxValue = 5;
        //}

        private void CreateColumn(string fieldName, string captionName, int width, DataTable dt, GridView gridView, Type type)
        {
            try
            {
                DevExpress.XtraGrid.Columns.GridColumn column = new DevExpress.XtraGrid.Columns.GridColumn();

                column.Name = fieldName;
                column.FieldName = fieldName;
                column.Caption = captionName;
                column.Visible = true;
                column.Width = width;

                if (width == 0)
                    column.Visible = false;

                gridView.Columns.Add(column);
                dt.Columns.Add(fieldName, type);
            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, e);
            }
        }

        private void DataClear()
        {
            pb_Image.Image = Properties.Resources.noImage;
            pb_Image.Refresh();

            //foreach (DataRow row in tableArr[0].Rows)
            //{
            //    for (int i = 2; i < 13; i++)
            //        row[i] = 0.0;

            //    row[(int)COL.Result] = 1;
            //}

            for (int i = 0; i < 2; i++)
                tableArr[i].Rows.Clear();

            for (int i = 0; i < 2; i++)
                gridViewArr[i].RefreshData();
        }

        public bool StartWeldAfterInspect(string dataPath, int modelNo)
        {
            string dataFile = Application.StartupPath + "\\results.dat";
            string imageFile = Application.StartupPath + "\\image.jpg";

            FileDelte(dataFile, imageFile);

            // 이미지 분석 프로세스 실행
            AppendLog("WeldAfterInspect Start");
            try
            {
                ProcessStartInfo startInfo = new ProcessStartInfo("WeldAfterInspect.exe");
                startInfo.WindowStyle = ProcessWindowStyle.Hidden;
                startInfo.WorkingDirectory = Application.StartupPath;

                string args = string.Format("{0} {1}", dataPath.Replace(" ", "?"), modelNo);
                //                string args = string.Format("{0} {1}", dataPath.Replace(" ", "?"), modelNo);
                AppendLog("Args: " + args);

                startInfo.Arguments = args;
                Process.Start(startInfo);
            }
            catch
            {
                AppendLog("WeldAfterInspect Fail");
                return false;
            }

            if (!FileSearch(new string[] { dataFile, imageFile }, iniManagement.iniModel.Process_AfterTimeOut))
            {
                AppendLog("WeldAfterInspect Wait TimeOut");
                return false;
            }

            Thread.Sleep(100);

            return true;
        }

        public bool Processing(string dataPath, string lotID, int modelNo, DateTime nowTime)
        {

            //if (!FileSearch(new string[] { dataPath }, 2000))
            //    return false;

            DataClear();

            string dataFile = Application.StartupPath + "\\results.dat";
            string imageFile = Application.StartupPath + "\\image.jpg";

            try
            {
                if (befLotID == lotID)
                {
                    productCount++;
                }
                else
                {
                    befLotID = lotID;
                    productCount = 1;
                }

                lbl_LotID.Text = string.Format("LotID: {0}     Count: {1}", befLotID, productCount);

                // PASS, REWORK, FAIL 판정 // 12번 째 Result // 10 번째 Type
                string result = "PASS";
                StreamReader sr = new StreamReader(dataFile, Encoding.UTF8);
                bool isRework = false;
                bool isFail = false;

                while (!sr.EndOfStream)
                {
                    string s = sr.ReadLine();
                    string[] tokken = s.Split(',');
                    //                    if (tokken.Length < 14)
                    if (tokken.Length < 7)
                        continue;

                    if (tokken[(int)COL.Result] == "2")
                    {
                        isRework = true;
                    }
                    else if (tokken[(int)COL.Result] == "3")
                    {
                        isFail = true;
                        break;
                    }
                }

                if (isFail)
                    result = "FAIL";
                else if (isRework)
                    result = "REWORK";

                sr.Close();
                sr.Dispose();

                Thread.Sleep(100);

                // 파일 복사
                //DateTime nowTime = DateTime.Now;
                string yyyy = nowTime.ToString("yyyy");
                string mm = nowTime.ToString("MM");
                string dd = nowTime.ToString("dd");

                // D:\Inspt_aft\ModelName\Year\Month\Day
                string csvForderPath = string.Format("{0}\\CSV\\{1}\\{2}\\{3}", iniManagement.iniModel.Process_SavePath, yyyy, mm, dd);
                if (!Directory.Exists(csvForderPath))
                    Directory.CreateDirectory(csvForderPath);

                saveCsvPath = string.Format("{0}\\{1}_{2}_{3}.csv", csvForderPath, nowTime.ToString("HH_mm_ss"), lotID, result);
                AppendLog("CSV: " + saveCsvPath);

                string imageForderPath = string.Format("{0}\\Image\\{1}\\{2}\\{3}", iniManagement.iniModel.Process_SavePath, yyyy, mm, dd);

                if (!Directory.Exists(imageForderPath))
                    Directory.CreateDirectory(imageForderPath);

                saveImgPath = string.Format("{0}\\{1}_{2}_{3}.jpg", imageForderPath, nowTime.ToString("HH_mm_ss"), lotID, result);
                AppendLog("Image: " + saveImgPath);

                File.Copy(dataFile, saveCsvPath);
                File.Copy(imageFile, saveImgPath);

                Thread.Sleep(100);

                FileDelte(dataFile, imageFile);

                CsvParsing(saveCsvPath);

                // Area Result Return
                //List<byte> plcWriteScoreList = new List<byte>();
                //short tmpPlcArea = 0;
                //short tmpValue = 0;
                //double tmpScore = 0;

                // PLC에 결과 값 쓰기
                //foreach (DataRow row in dataTable.Rows)
                //{
                //    tmpScore = double.Parse(row[iniManagement.iniModel.Process_MesIndex].ToString());

                //    // Write 범위 0~99
                //    if (iniManagement.iniModel.Process_Line == (int)LINE.J190)
                //    {
                //        // 소수점 1자리고 변환
                //        tmpPlcArea = (short)(Math.Truncate(tmpScore * 10));
                //    }
                //    else if (iniManagement.inspectionModel.WeldAfterInspect_UseDeepLearning == 1)
                //    {
                //        tmpValue = (short)(tmpScore * 100);
                //        if (tmpValue > 99)
                //            tmpValue = 99;
                //    }
                //    else
                //    {
                //        tmpValue = (short)tmpScore;
                //        if (tmpValue < 0)
                //            tmpPlcArea = 0;
                //        else if (tmpValue > 99)
                //            tmpPlcArea = 99;
                //        else
                //            tmpPlcArea = tmpValue;
                //    }

                //    plcWriteScoreList.AddRange(BitConverter.GetBytes(tmpPlcArea));
                //}

                //return plcWriteScoreList.ToArray();

                return true;
            }
            catch (Exception e)
            {
                AppendLog("Processing Fail2: " + e.Message);
                return false;
            }
        }

        private void CsvParsing(string csvFile)
        {
            DataClear();

            int index = -1;
            int rowIndex = 0;
            StreamReader sr = new StreamReader(csvFile, Encoding.UTF8);
            // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
            inspectionResultArr = new short[m_slot * (m_layer / 2)];

            //radialChartList.Clear();
            //tangentialChartList.Clear();

            okCount = 0;
            reworkCount = 0;
            failCount = 0;

            totalResult = (int)RESPONSE.OK;

            //21.05.20 후 검사 result.dat 결과파일 포맷 변경으로 인한 수정
            while (!sr.EndOfStream)
            {
                index++;

                string s = sr.ReadLine();
                string[] tokken = s.Split(',');
                //if (tokken.Length < (int)COL.Tangential)
                //    continue;

                if (index == 0)
                    continue;

                if (tokken[(int)COL.Result] == "1")
                    okCount++;
                else if (tokken[(int)COL.Result] == "2")
                    reworkCount++;
                else if (tokken[(int)COL.Result] == "3")
                    failCount++;

                //for (int i = (int)COL.Result; i < (int)COL.Radial; i++)
                //    tableArr[0].Rows[rowIndex][i] = tokken[i];

                // Slot, Pin, Height Avg, Height 1,	Height 2, Height C, Gradient, Dev, Width, Height Tan, Type, Area, Result, Radial, Tangential	
                DataRow row = tableArr[0].NewRow();
                for (int i = 0; i < (int)COL.Col2; i++)
                {
                    row[i] = tokken[i];
                    if(i == (int)COL.Result)
                    {
                        if (befInspModelList[rowIndex].result == 2) // sjyi 2024.05.23 재용접 포인트 추가
                        {
                            if (tokken[(int)COL.Result] == "1")
                            {
                                row[i] = 4; // 재용접 OK
                            }
                            else
                            {
                                row[i] = 5; // 재용접 NG
                            }
                        }

                    }
                }
                tableArr[0].Rows.Add(row);

                if (tokken[(int)COL.Result] != "1")
                {
                    row = tableArr[1].NewRow();
                    for (int i = 0; i < (int)COL.Col2; i++)
                        row[i] = tableArr[0].Rows[rowIndex][i];

                    tableArr[1].Rows.Add(row);
                }

                if (befInspModelList[rowIndex].result == 2) // sjyi 2024.05.23 재용접 포인트 추가
                {
                    if(tokken[(int)COL.Result] == "1")
                    {
                        inspectionResultArr[rowIndex] = 4; // 재용접 OK
                    }
                    else
                    {
                        inspectionResultArr[rowIndex] = 5; // 재용접 NG
                    }
                }
                else
                {
                    inspectionResultArr[rowIndex] = short.Parse(tokken[(int)COL.Result]);
                }


                // TODO: 확인 후 진행
                //// 결과 보관
                //string key = tokken[(int)COL.Slot] + " " + tokken[(int)COL.Layer];
                //if (aftPointDict.ContainsKey(key))
                //    aftPointDict[key] = int.Parse(tokken[(int)COL.Result]);

                //// Chart
                //string[] chartTokken = tokken[13].Split(';');   // Radial
                //List<float> tmpChartList = new List<float>();
                //foreach (string tmpS in chartTokken)
                //{
                //    if (string.IsNullOrWhiteSpace(tmpS))
                //        continue;

                //    tmpChartList.Add(float.Parse(tmpS));
                //}
                //radialChartList.Add(tmpChartList.ToArray());

                //chartTokken = tokken[14].Split(';');   // Tangential
                //tmpChartList.Clear();
                //foreach (string tmpS in chartTokken)
                //{
                //    if (string.IsNullOrWhiteSpace(tmpS))
                //        continue;

                //    tmpChartList.Add(float.Parse(tmpS));
                //}
                //tangentialChartList.Add(tmpChartList.ToArray());

                rowIndex++;
            }

            sr.Close();
            sr.Dispose();

            // sjyi 2023.07.20  화면 표시 항목 변경, 기존 : PASS Count -> 변경 : 재용접 Count
            // 이 부분도 현장 적용을 위해, 임의 수정하였으나, 추후, 환경설정 등으로 변경 필요.
            // sjyi 2023.10.18 환경설정 적용 완료
            if (iniManagement.iniModel.Process_MakePreData)
            {
                lbl_Result.Text = string.Format("POINT: {0} REWORK: {1} FAIL: {2}", m_nReworkCnt.ToString("D2"), reworkCount.ToString("D2"), failCount.ToString("D2"));
            }
            else
            {
                lbl_Result.Text = string.Format("PASS: {0} REWORK: {1} FAIL: {2}", okCount.ToString("D2"), reworkCount.ToString("D2"), failCount.ToString("D2"));
            }
            //}

            if (iniManagement.iniModel.Process_ResultStandard == 0)
            {
                if (reworkCount > 0)
                    totalResult = (int)RESPONSE.NG;
                else if (failCount > 0)
                    totalResult = (int)RESPONSE.ALARM;
            }
            else
            {
                if (failCount > 0)
                    totalResult = (int)RESPONSE.ALARM;
                else if (reworkCount > 0)
                    totalResult = (int)RESPONSE.NG;
            }

            if (reworkCount == 0 && failCount == 0)
                lbl_Result.ForeColor = Color.Green;
            else
                lbl_Result.ForeColor = Color.Red;

            // Image Search
            string[] tmpTokken = csvFile.Split('\\');
            string imageFile = string.Format("{0}\\Image\\{1}\\{2}\\{3}\\{4}.jpg",
                iniManagement.iniModel.Process_SavePath, tmpTokken[tmpTokken.Length - 4], tmpTokken[tmpTokken.Length - 3],
                tmpTokken[tmpTokken.Length - 2], tmpTokken.Last().Split('.').First());
            UpdateImage(imageFile);

            // Draw Procsssing
            ProcessingDrawResult();

            //// Image Search
            //// D:\InsptAft\ModelName\Year\Month\Day\HHmmss_LOTID.csv
            //// D:\InsptAft\ModelName\Year\Month\Day\HHmmss_LOTID.png

            ////string[] tmpTokken = csvFile.Split('.');
            ////string imageFile = string.Format("{0}\\Image\\{1}\\{2}\\{3}\\{4}.jpg",
            ////    iniManagement.iniModel.Process_SavePath, tmpTokken[tmpTokken.Length - 4], tmpTokken[tmpTokken.Length - 3],
            ////    tmpTokken[tmpTokken.Length - 2], tmpTokken.Last().Split('.').First());
            //string imageFile = csvFile.Split('.')[0] + ".png";
            //UpdateImage(imageFile);

            //// Draw Procsssing
            //ProcessingDrawResult();
        }

        private bool FileSearch(string[] filePath, int timeOut)
        {
            foreach (string str in filePath)
                AppendLog(str);

            bool[] tmpResult = new bool[filePath.Length];

            int time = 0;
            while (true)
            {
                try
                {
                    if (time >= timeOut)
                    {
                        AppendLog("File Search Fail");
                        return false;
                    }

                    for (int i = 0; i < filePath.Length; i++)
                    {
                        if (File.Exists(filePath[i]))
                            tmpResult[i] = true;
                        else
                            tmpResult[i] = false;
                    }

                    bool isTotalResult = true;

                    foreach (bool result in tmpResult)
                    {
                        if (!result)
                            isTotalResult = false;
                    }

                    if (isTotalResult)
                        return true;
                }
                catch (Exception e)
                {
                    AppendLog("FileSearch: " + e.Message);
                    return false;
                }
                finally
                {
                    time += 100;
                    Thread.Sleep(100);
                }
            }
        }

        //private bool FileSearch(string imgPath, string dataFile, int timeOut)
        //{
        //    AppendLog(imgPath);
        //    AppendLog(dataFile);

        //    int time = 0;
        //    while (true)
        //    {
        //        try
        //        {
        //            if (time >= timeOut)
        //            {
        //                AppendLog("File Search Fail");                        
        //                return false;
        //            }

        //            if (File.Exists(imgPath) && File.Exists(dataFile))
        //                return true;
        //        }
        //        catch (Exception e)
        //        {
        //            AppendLog("FileSearch: " + e.Message);
        //            return false;
        //        }
        //        finally
        //        {
        //            time += 100;
        //            Thread.Sleep(100);
        //        }
        //    }
        //}

        private void FileDelte(string dataFile, string imageFile)
        {
            int count = 0;
            while (true)
            {
                try
                {
                    if (count == 10)
                    {
                        AppendLog("File Delete Fail");
                        break;
                    }

                    if (File.Exists(dataFile))
                        File.Delete(dataFile);

                    if (File.Exists(imageFile))
                        File.Delete(imageFile);

                    break;
                }
                catch (Exception e)
                {
                    AppendLog("File Delete: " + e.Message);
                }
                finally
                {
                    count++;
                    Thread.Sleep(100);
                }
            }
        }

        private void ProcessingDrawResult()
        {
            try
            {
                //drawResultArr = new short[iniModel.WeldAfterInspect_NumberofSlots];
                //int index = 0;
                //short error = 0;
                //int spaceCount = (iniModel.WeldAfterInspect_NumberofLayers / 2);
                //for (int i = 0; i < inspectionResultArr.Length; i += spaceCount)
                //{
                //    error = 0;
                //    error += inspectionResultArr[i];
                //    error += inspectionResultArr[i + 1];

                //    if (error > (int)PLC_RESPONSE.REWORK)
                //        error = (int)PLC_RESPONSE.REWORK;
                //    else
                //        error = (int)PLC_RESPONSE.PASS;

                //    drawResultArr[index++] = error;
                //}

                AppendLog(string.Format("SlotNo : {0}, LayerNo : {1}", m_slot, m_layer));

                if (inspectionResultArr == null)
                    return;

                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                drawResultArr = new short[m_slot];
                int index = 0;
                short error = 0;

                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                int spaceCount = (m_layer / 2);

                for (int i = 0; i < inspectionResultArr.Length; i += spaceCount)
                {
                    error = 0;
                    for (int j = 0; j < spaceCount; j++)
                        error += inspectionResultArr[i + j];

                    if (error > spaceCount)
                        drawResultArr[index++] = (int)RESPONSE.NG;
                    else
                        drawResultArr[index++] = (int)RESPONSE.OK;
                }

                Draw();
            }
            catch (Exception e)
            {
                AppendLog("ProcessingDrawResult: " + e.Message);
            }
        }

        //public void SetBeforeInspectionData(byte[] recvBefore)
        //{
        //    beforeInspectionData.Clear();
        //    if(recvBefore == null)
        //        return;

        //    for (int i = 0; i < recvBefore.Length; i += 2)
        //        beforeInspectionData.Add(BitConverter.ToInt16(recvBefore, i));
        //}

        public byte[] GetWriteResultData()
        {
            try
            {
                //if (befInspModelList.Count == 0)
                //    return null;

                //List<int> okPointList = new List<int>();
                //int count = iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots * (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2);
                //for (int i = 0; i < count; i++)
                //{
                //    if (befInspModelList[i].result == 2)    // 전 검사에서 NG 일 경우 Pass로 변경
                //    {
                //        inspectionResultArr[i] = (int)RESPONSE.OK;
                //        okPointList.Add(count);
                //    }
                //}

                // TODO: 확인 후 진행
                //// 전검사에서 NG 난 항목 값 변경
                //List<string> changeKeyList = new List<string>();
                //int idx = 0;
                //foreach(KeyValuePair<string, int> dict in aftPointDict)
                //{
                //    if (okPointList.Contains(idx))
                //        changeKeyList.Add(dict.Key);

                //    idx++;
                //}

                //foreach (string key in changeKeyList)
                //    aftPointDict[key] = (int)RESPONSE.PASS;

                List<byte> dbList = new List<byte>();
                foreach (short s in inspectionResultArr)
                    dbList.AddRange(BitConverter.GetBytes(s));

                return dbList.ToArray();
            }
            catch
            {
                return null;
            }
        }

        public string GetAftErrorPoint()
        {
            aftErrorPoint = string.Empty;
            foreach (KeyValuePair<string, int> dict in aftPointDict.Where(e => e.Value != (int)RESPONSE.OK))
            {
                if (dict.Value == (int)RESPONSE.NG)
                    aftErrorPoint += (dict.Key + "  REWORK\n");
                else if (dict.Value == (int)RESPONSE.ALARM)
                    aftErrorPoint += (dict.Key + "  FAIL\n");
                else if (dict.Value == (int)RESPONSE.PASS)
                    aftErrorPoint += (dict.Key + "  PASS\n");
                else
                    aftErrorPoint += (dict.Key + "  NONE\n");
            }

            return aftErrorPoint;
        }

        private void UpdateImage(string imagePath)
        {
            if (!File.Exists(imagePath))
            {
                AppendLog("Image Not Found: " + imagePath);
                return;
            }

            Image img = Image.FromFile(imagePath);
            //img.RotateFlip(RotateFlipType.RotateNoneFlipY);
            pb_Image.Image = GetHalfImg(img);
            pb_Image.Refresh();
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void Draw(Graphics g = null)
        {
            try
            {
                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                if (m_slot < 96)
                    return;

                if (g == null)
                    g = pb_Image.CreateGraphics();

                if (drawResultArr == null)
                    return;

                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                double angle = 360.0 / m_slot;

                //if (slot > 24)
                //    angle = 360.0 / slot;
                //else
                //    angle = 180.0 / slot;

                double x, y;

                float centerX = pb_Image.Width / 2;
                float centerY = pb_Image.Height / 2;

                float drawRange, drawAngle, drawXoffset, drawYoffset;

                // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
                // sjyi 2023.07.25 여기도 추가 수정 필요 할 듯. 2차용접기는 Recipe가 2개만 있음.
                if (m_rcp < 3)
                {
                    drawRange = iniManagement.iniModel.Model77_Range;
                    drawAngle = iniManagement.iniModel.Model77_Angle;
                    drawXoffset = iniManagement.iniModel.Model77_XOffSet;
                    drawYoffset = iniManagement.iniModel.Model77_YOffSet;
                }
                else
                {
                    drawRange = iniManagement.iniModel.Model76_Range;
                    drawAngle = iniManagement.iniModel.Model76_Angle;
                    drawXoffset = iniManagement.iniModel.Model76_XOffSet;
                    drawYoffset = iniManagement.iniModel.Model76_YOffSet;
                }

                //for (int i = 0; i < slot; i++)
                //{
                //    double radius = drawRange / 2.0;
                //    double dA = 90 + i * angle + drawAngle;

                //    UserUtil.P2R(radius - 4 / 2, dA * D2R, out x, out y);

                //    g.DrawString((i + 1).ToString(), drawFont, drawResultArr[i] == (int)RESPONSE.OK ? okBrush : ngBrush,
                //        (float)x + centerX + drawXoffset, -(float)y + centerY + drawYoffset);
                //}
            }
            catch { }
        }



        private void UcInspection_Resize(object sender, EventArgs e)
        {
            //pnl_Image.Refresh();
        }

        private void pnl_Image_Paint(object sender, PaintEventArgs e)
        {
            Draw(e.Graphics);
        }

        private void gridView_RowStyle(object sender, DevExpress.XtraGrid.Views.Grid.RowStyleEventArgs e)
        {
            try
            {
                string result = gridView.GetRowCellDisplayText(e.RowHandle, gridView.Columns["Result"]);
                //if (result == "2" || result == "3")
                if (result != "1" && result !="4")
                {
                    e.Appearance.BackColor = Color.Red;
                    e.Appearance.ForeColor = Color.White;
                }
            }
            catch { }
        }

        private void gridView_RowCellStyle(object sender, RowCellStyleEventArgs e)
        {
            try
            {
                if (e.Column.FieldName == "Slot")
                {
                    e.Appearance.BackColor = Color.Gray;
                    e.Appearance.ForeColor = Color.White;
                }
            }
            catch { }
        }

        //private void gridView_RowCellClick(object sender, RowCellClickEventArgs e)
        //{
        //    try
        //    {
        //        //radialTable.Rows.Clear();
        //        //tangentialTable.Rows.Clear();

        //        //if (radialChartList.Count >= e.RowHandle)
        //        //{
        //        //    float[] profileDataArr = radialChartList[e.RowHandle];
        //        //    for (int i = 0; i < profileDataArr.Length; i++)
        //        //        radialTable.Rows.Add("Pixcel Value", i, profileDataArr[i]);

        //        //    cc_Radial.RefreshData();
        //        //}

        //        //if (tangentialChartList.Count >= e.RowHandle)
        //        //{
        //        //    float[] profileDataArr = tangentialChartList[e.RowHandle];

        //        //    for (int i = 0; i < profileDataArr.Length; i++)
        //        //        tangentialTable.Rows.Add("Pixcel Value", i, profileDataArr[i]);

        //        //    cc_Tangential.RefreshData();
        //        //}
        //    }
        //    catch (Exception ex)
        //    {
        //        AppendLog("RowCellClick Error: " + ex.Message);
        //    }

        //}

        private void gridView_CellMerge(object sender, DevExpress.XtraGrid.Views.Grid.CellMergeEventArgs e)
        {
            CellMerge(sender, e, 0);
        }

        private void gridView_NG_CellMerge(object sender, CellMergeEventArgs e)
        {
            CellMerge(sender, e, 1);
        }

        private void CellMerge(object sender, DevExpress.XtraGrid.Views.Grid.CellMergeEventArgs e, int idx)
        {
            try
            {
                if (e.Column.FieldName == "Slot")
                {
                    var dr1 = gridViewArr[idx].GetDataRow(e.RowHandle1); //위에 행 정보
                    var dr2 = gridViewArr[idx].GetDataRow(e.RowHandle2); //아래 행 정보
                    if (dr1 == null || dr2 == null)
                        return;

                    e.Merge = dr1["Slot"].ToString().Equals(dr2["Slot"].ToString());
                }
                else
                {
                    e.Merge = false;
                }

                e.Handled = true;
            }
            catch (Exception ex)
            {
                //AppendLog("gridView_CellMerge: " + ex.Message);
            }
        }

        private void pb_Open_Click(object sender, EventArgs e)
        {
            string defaultPath = string.Format("{0}\\CSV\\{1}\\{2}\\{3}", iniManagement.iniModel.Process_SavePath,
               DateTime.Now.ToString("yyyy"), DateTime.Now.ToString("MM"), DateTime.Now.ToString("dd"));

            if (!Directory.Exists(defaultPath))
                Directory.CreateDirectory(defaultPath);

            OpenFileDialog openFileDlg = new OpenFileDialog();
            openFileDlg.InitialDirectory = defaultPath;
            openFileDlg.DefaultExt = "CSV";
            openFileDlg.Filter = "CSV Files (*.csv)|*.csv";
            if (openFileDlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    lbl_LotID.Text = "LOTID: ";

                    AppendLog("File Load: " + openFileDlg.FileName);
                    CsvParsing(openFileDlg.FileName);
                }
                catch (Exception ex)
                {
                    AppendLog("File Open Error: " + ex.Message);
                }

            }
            ////string defaultPath = string.Format("{0}\\CSV\\{1}\\{2}\\{3}", iniManagement.iniModel.Process_SavePath,
            ////    DateTime.Now.ToString("yyyy"), DateTime.Now.ToString("MM"), DateTime.Now.ToString("dd"));

            ////if (!Directory.Exists(defaultPath))
            ////    Directory.CreateDirectory(defaultPath);

            //OpenFileDialog openFileDlg = new OpenFileDialog();
            ////openFileDlg.InitialDirectory = defaultPath;
            //openFileDlg.DefaultExt = "CSV";
            //openFileDlg.Filter = "CSV Files (*.csv)|*.csv";
            //if (openFileDlg.ShowDialog() == DialogResult.OK)
            //{
            //    try
            //    {
            //        lbl_LotID.Text = "LOTID: ";

            //        AppendLog("File Load: " + openFileDlg.FileName);
            //        CsvParsing(openFileDlg.FileName);
            //    }
            //    catch (Exception ex)
            //    {
            //        AppendLog("File Open Error: " + ex.Message);
            //    }
            //}
        }

        public void AppendLog(string msg)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate ()
                {
                    CheckTextBox(tbx_Log, msg);
                }));
            }
            else
            {
                CheckTextBox(tbx_Log, msg);
            }


            LogManager.WriteLog(LogType.Information, msg);
        }

        //public void AppendFrameLog(string msg)
        //{
        //    CheckTextBox(tbx_FrameLog, msg);
        //    FrameLogManager.WriteLog(LogType.Information, msg);
        //}

        private void CheckTextBox(TextBox textBox, string msg)
        {
            lock (this)
            {
                if (textBox.Lines.Length > 50)
                {
                    int numOfLines = 1;
                    var lines = textBox.Lines;
                    var newLines = lines.Skip(numOfLines);

                    textBox.Lines = newLines.ToArray();
                }

                textBox.AppendText(string.Format("{0} - {1}", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"), msg + Environment.NewLine));
            }
        }

        public void GridViewRefreshData()
        {
            gridView.RefreshData();
            pb_Image.Refresh();
        }

        private void gridView_CustomDrawCell(object sender, DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventArgs e)
        {
            if (e.Column.Caption == "Result")
            {
                if (e.CellValue.ToString() == "1")
                    e.DisplayText = "PASS";
                else if (e.CellValue.ToString() == "2")
                    e.DisplayText = "REWORK";
                else if (e.CellValue.ToString() == "3")
                    e.DisplayText = "FAIL";
                else if (e.CellValue.ToString() == "4")
                    e.DisplayText = "RW_PASS";
                else if (e.CellValue.ToString() == "5")
                    e.DisplayText = "RW_NG";

            }
        }

        private void pictureBox_Paint(object sender, PaintEventArgs e)
        {
            Draw(e.Graphics);
        }

        private Image GetHalfImg(Image img)
        {
            // sjyi 2023.07.20  변수명 변경, 멤버 변수 구분하기 위해 m_ 추가 
            if (m_slot > 24)
                return img;

            if (img != null)
            {
                Bitmap croppedBitmap = new Bitmap(img);


                if (iniManagement.iniModel.Model77_Slot == 96 || iniManagement.iniModel.Model76_Slot == 96)
                {
                    croppedBitmap = croppedBitmap.Clone(
                                new Rectangle(1000, 0, img.Width - 2300, (img.Height / 2) - 1500), System.Drawing.Imaging.PixelFormat.DontCare);
                }
                else if (iniManagement.iniModel.Model77_Slot == 24 || iniManagement.iniModel.Model76_Slot == 24)
                {
                    croppedBitmap = croppedBitmap.Clone(
                          new Rectangle(0, 0, img.Width, (img.Height / 2 + 200)), System.Drawing.Imaging.PixelFormat.DontCare);
                }

                return croppedBitmap;
            }

            return img;
        }

        // sjyi 2023.07.20  2차 용접기 재용접 포인트만 검사하기 위해 만든 함수.
        public void MakeDataFile(string strLotID, bool bWriteFile, byte[] ReadResult = null)
        {
            // 메모리 생성
            int nResultSize = m_slot * (m_layer / 2);
            inspectionResultArr = new short[nResultSize];
            int nIndex = 0;
            int nSlot = 1;
            int nLayer = 1;
            string strResult = "ok";
            double dProb = 1;

            // 메모리(Inspection Result Array)에 PLC 데이터 업데이트
            // Make byte to short

            int nByteIndex = 0;
            if (ReadResult != null)
            {
                for (int i = 0; i < nResultSize; i++)
                {
                    nByteIndex = i * 2;

                    if (nByteIndex < ReadResult.Length)
                    {
                        inspectionResultArr[i] = BitConverter.ToInt16(ReadResult, nByteIndex);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // CSV 파일 Path 설정
            string dataFile = "";
            if(bWriteFile)
            {
                dataFile = Application.StartupPath + "\\previous_results.csv";
            }
            else
            {
                dataFile = Application.StartupPath + "\\previous_results_temp.csv";
            }

            // File Open
            StreamWriter sr = new StreamWriter(dataFile, false, Encoding.UTF8);

            // Make Header
            string strLine;
            strLine = "disc:,previous stator's insp. result!,guard";
            sr.WriteLine(strLine);
            //strLine = "lotid:,Bin,guard";
            if (strLotID.Length == 0)
            {
                strLotID = "Bin";
            }
            strLine = "lotid:," + strLotID + ",guard";
            sr.WriteLine(strLine);
            strLine = "index,slot,layer(s~e),result,prob";
            sr.WriteLine(strLine);

            // Make Data String Data & Write a line
            int nReworkCnt = 0;
            befInspModelList.Clear(); // sjyi 2024.05.23 이전검사 데이터 초기화
            for (int i = 0; i < nResultSize; i++)
            {
                nIndex = i + 1;
                if (inspectionResultArr[i] == 2)
                {
                    strResult = "rework";
                    nReworkCnt++;
                }
                else
                {
                    strResult = "ok";
                }
                dProb = 1.0;

                // sjyi 2024.05.23 추가 ---> 재용접 포인트 확인용
                BefInspModel model = new BefInspModel();
                model.slot = nSlot;
                model.layer = (nLayer / 2).ToString();
                model.result = inspectionResultArr[i];
                befInspModelList.Add(model);
                // sjyi 2024.05.23 추가 <--- 재용접 포인트 확인용

                strLine = string.Format("{0},{1},{2}~{3},{4},{5}", nIndex, nSlot, nLayer, nLayer + 1, strResult, dProb);
                sr.WriteLine(strLine);

                nLayer = nLayer + 2;
                if (nLayer > m_layer)
                {
                    nSlot++;
                    nLayer = 1;
                }
            }

            m_nReworkCnt = nReworkCnt;

            // Log
            AppendLog(string.Format("Make Previous Data - ReworkCnt : {0}, LotID : {1}", nReworkCnt, strLotID));

            // File Close
            sr.Close();

            string dataFileBackUp = Application.StartupPath + "\\previous_results_backup.csv";
            File.Copy(dataFile, dataFileBackUp, true);
        }
    }
}