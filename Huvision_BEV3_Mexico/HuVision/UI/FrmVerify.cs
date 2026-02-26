using DevExpress.XtraGrid.Views.BandedGrid;
using DevExpress.XtraGrid.Views.Grid;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HuVision.UI
{
    public partial class FrmVerify : Form
    {
        private DataTable befPstTable = new DataTable();
        private DataTable lstPstTable = new DataTable();

        private enum LIST_ORDER { AX = 0, AY, BX, BY, C, JUMP }
        private enum CMES_BEF { SLOT_LAYER = 0, H1, H2, H3, H4, AX, AY, BX, BY, C, THETA }

        private double cellValue = 0;

        private IniManagement iniManagement;
        private int modelNo = 0;

        private enum BEF_PST
        {
            SLOT = 0, LAYER = 1,
            AX_AVG, AX_MIN, AX_MAX, AX_ABS,
            AY_AVG, AY_MIN, AY_MAX, AY_ABS,
            BX_AVG, BX_MIN, BX_MAX, BX_ABS,
            BY_AVG, BY_MIN, BY_MAX, BY_ABS,
            C_AVG, C_MIN, C_MAX, C_ABS,
            JUMP_AVG, JUMP_MIN, JUMP_MAX, JUMP_ABS
        }

        private enum LST_PST { SLOT = 0, LAYER = 1, H1, H2, H3, H4, AX, AY, AR, BX, BY, BR, C, JUMP }

        public FrmVerify(int modelNo)
        {
            InitializeComponent();

            this.modelNo = modelNo;

            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            iniManagement = IniManagement.Instance();

            if (iniManagement.iniModel.Process_Line == (int)LINE.FORD)
            {
                btn_Er11Create.Visible = true;
                btn_Ef21Create.Visible = true;
            }
            else
            {
                btn_Create.Visible = true;
            }
        }

        private void FrmVerify_Load(object sender, EventArgs e)
        {
            gc_BefPst.DataSource = befPstTable;
            gc_LstPst.DataSource = lstPstTable;

            CreateBandedColumn(gv_BefPst, befPstTable, " ", new string[] { "Slot", "Layer" }, new string[] { "Slot", "Layer" }, typeof(string), new int[] { 15, 15 });
            CreateBandedColumn(gv_BefPst, befPstTable, "AX", new string[] { "Avg", "Min", "Max", "Min-Max" }, new string[] { "axAvg", "axMin", "axMax", "axMin-Max" }, typeof(float), new int[] { 20, 20, 20, 20 });
            CreateBandedColumn(gv_BefPst, befPstTable, "AY", new string[] { "Avg", "Min", "Max", "Min-Max" }, new string[] { "ayAvg", "ayMin", "ayMax", "ayMin-Max" }, typeof(float), new int[] { 20, 20, 20, 20 });
            CreateBandedColumn(gv_BefPst, befPstTable, "BX", new string[] { "Avg", "Min", "Max", "Min-Max" }, new string[] { "bxAvg", "bxMin", "bxMax", "bxMin-Max" }, typeof(float), new int[] { 20, 20, 20, 20 });
            CreateBandedColumn(gv_BefPst, befPstTable, "BY", new string[] { "Avg", "Min", "Max", "Min-Max" }, new string[] { "byAvg", "byMin", "byMax", "byMin-Max" }, typeof(float), new int[] { 20, 20, 20, 20 });
            CreateBandedColumn(gv_BefPst, befPstTable, "C", new string[] { "Avg", "Min", "Max", "Min-Max" }, new string[] { "cAvg", "cMin", "cMax", "cMin-Max" }, typeof(float), new int[] { 20, 20, 20, 20 });
            CreateBandedColumn(gv_BefPst, befPstTable, "JUMP", new string[] { "Avg", "Min", "Max", "Min-Max" }, new string[] { "jAvg", "jMin", "jMax", "jMin-Max" }, typeof(float), new int[] { 20, 20, 20, 20 });

            CreateColumn("Slot", "Slot", 50, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("Layer", "Layer", 50, lstPstTable, gv_LstPst, typeof(string));

            CreateColumn("1H", "1H", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("2H", "2H", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("3H", "3H", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("4H", "4H", 70, lstPstTable, gv_LstPst, typeof(string));

            CreateColumn("AX", "AX", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("AY", "AY", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("AR", "R", 70, lstPstTable, gv_LstPst, typeof(string));

            CreateColumn("BX", "BX", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("BY", "BY", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("BR", "R", 70, lstPstTable, gv_LstPst, typeof(string));

            CreateColumn("C", "C", 70, lstPstTable, gv_LstPst, typeof(string));
            CreateColumn("JUMP", "JUMP", 70, lstPstTable, gv_LstPst, typeof(string));

            ProcessingBefPst();
        }

        private void CreateBandedColumn(DevExpress.XtraGrid.Views.BandedGrid.BandedGridView bandedGridView, DataTable dt,
           string gridBandCaption, string[] captionNames, string[] fieldNames, Type type, int[] fixedWidth = null)
        {
            DevExpress.XtraGrid.Views.BandedGrid.GridBand gridBand = new DevExpress.XtraGrid.Views.BandedGrid.GridBand();
            gridBand.Name = gridBandCaption;
            gridBand.Caption = gridBandCaption;
            gridBand.OptionsBand.AllowMove = false;

            DevExpress.XtraGrid.Views.BandedGrid.BandedGridColumn[] bandedColumns = new DevExpress.XtraGrid.Views.BandedGrid.BandedGridColumn[captionNames.Length];
            for (int i = 0; i < captionNames.Length; i++)
            {
                bandedColumns[i] = (DevExpress.XtraGrid.Views.BandedGrid.BandedGridColumn)bandedGridView.Columns.AddField(captionNames[i]);
                bandedColumns[i].Caption = captionNames[i];
                bandedColumns[i].FieldName = fieldNames[i];
                bandedColumns[i].OwnerBand = gridBand;
                bandedColumns[i].Visible = true;
                bandedColumns[i].OptionsColumn.AllowMove = false;
                bandedColumns[i].OptionsColumn.FixedWidth = true;

                if (fixedWidth != null)
                {
                    if (fixedWidth[i] == 0)
                        bandedColumns[i].Visible = false;
                    else
                        bandedColumns[i].Width = fixedWidth[i];

                }

                dt.Columns.Add(fieldNames[i], type);
            }

            bandedGridView.Bands.Add(gridBand);
        }

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

            }
        }

        public void ProcessingBefPst()
        {
            try
            {
                string path = string.Empty;
                if (modelNo == 1)
                    path = Application.StartupPath + "\\ER11befPst.dat";
                else if (modelNo == 2)
                    path = Application.StartupPath + "\\EF21befPst.dat";
                else
                    path = Application.StartupPath + "\\befPst.dat";

                if (!File.Exists(path))
                {
                    MessageBox.Show(path + " 파일이 없습니다");
                    return;
                }

                befPstTable.Rows.Clear();
                using (StreamReader sr = new StreamReader(path, Encoding.UTF8))
                {
                    bool isFirst = true;
                    while (!sr.EndOfStream)
                    {
                        string[] tokken = sr.ReadLine().Split(',');
                        if (isFirst)
                        {
                            isFirst = false;
                            continue;
                        }

                        DataRow row = befPstTable.NewRow();
                        row.ItemArray = tokken.Cast<object>().ToArray();
                        befPstTable.Rows.Add(row);
                    }
                }
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                LogManager.WriteLog(LogType.Error, "ProcessingBefPst" + e.Message);
            }

        }

        private void btn_Create_Click(object sender, EventArgs e)
        {
            DevExpress.XtraEditors.SimpleButton btn = sender as DevExpress.XtraEditors.SimpleButton;
            int no = 0;
            if (btn.Name == "btn_Er11Create")
                no = 1;
            else if (btn.Name == "btn_Ef21Create")
                no = 2;

            if (SaveBefPstFile(tbx_Path.Text, no))
                ProcessingBefPst();
        }

        public bool SaveBefPstFile(string path, int model)
        {
            try
            {
                if (!Directory.Exists(path))
                {
                    MessageBox.Show(path + " 해당 경로가 없습니다.");
                    return false;
                }

                //Dictionary<string, List<List<double>>> dataDict = new Dictionary<string, List<List<double>>>();
                // 초기화
                // Key: Slot_Layer
                // Value: Key-명칭, value-값 리스트
                Dictionary<string, Dictionary<string, List<double>>> dataDict = new Dictionary<string, Dictionary<string, List<double>>>();
                for (int i = 1; i < iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots + 1; i++)
                {
                    for (int j = 0; j < (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2); j++)
                    {
                        string layer = string.Empty;
                        switch (j)
                        {
                            case 0: layer = i + "_1/2"; break;
                            case 1: layer = i + "_3/4"; break;
                            case 2: layer = i + "_5/6"; break;
                            case 3: layer = i + "_7/8"; break;
                        }
                        Dictionary<string, List<double>> layerList = new Dictionary<string, List<double>>();
                        //List<List<double>> layerList = new List<List<double>>();
                        //layerList.Add(new List<double>());   // ax
                        //layerList.Add(new List<double>());   // ay
                        //layerList.Add(new List<double>());   // bx
                        //layerList.Add(new List<double>());   // by
                        //layerList.Add(new List<double>());   // c
                        //layerList.Add(new List<double>());   // jump
                        layerList.Add("AX", new List<double>());
                        layerList.Add("AY", new List<double>());
                        layerList.Add("BX", new List<double>());
                        layerList.Add("BY", new List<double>());
                        layerList.Add("C", new List<double>());
                        layerList.Add("JUMP", new List<double>());
                        dataDict.Add(layer, layerList);
                    }
                }

                // 파일 분석 및 보관
                List<string[]> inspDataList = new List<string[]>();
                DirectoryInfo di = new DirectoryInfo(path);
                FileInfo[] fi = di.GetFiles("*.csv", SearchOption.AllDirectories);
                foreach (FileInfo f in fi)
                {
                    inspDataList.Clear();
                    using (StreamReader sr = new StreamReader(f.FullName, Encoding.UTF8))
                    {
                        bool isFirst = true;
                        while (!sr.EndOfStream)
                        {
                            string str = sr.ReadLine();
                            if (isFirst)
                            {
                                isFirst = false;
                                continue;
                            }
                            inspDataList.Add(str.Split(','));
                        }
                    }

                    for (int i = 0; i < inspDataList.Count; i++)
                    {
                        if (inspDataList[i][(int)CMES_BEF.AX] == "0" || inspDataList[i][(int)CMES_BEF.AY] == "0" ||
                            inspDataList[i][(int)CMES_BEF.BX] == "0" || inspDataList[i][(int)CMES_BEF.BY] == "0")
                            continue;

                        if (dataDict.ContainsKey(inspDataList[i][(int)CMES_BEF.SLOT_LAYER]))
                        {
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]][(int)LIST_ORDER.AX].Add(float.Parse(tokken[(int)CMES_BEF.AX]));
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]][(int)LIST_ORDER.AY].Add(float.Parse(tokken[(int)CMES_BEF.AY]));
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]][(int)LIST_ORDER.BX].Add(float.Parse(tokken[(int)CMES_BEF.BX]));
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]][(int)LIST_ORDER.AY].Add(float.Parse(tokken[(int)CMES_BEF.BY]));
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]][(int)LIST_ORDER.C].Add(float.Parse(tokken[(int)CMES_BEF.C]));
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]][(int)LIST_ORDER.JUMP].Add(float.Parse(tokken[8]));

                            dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["AX"].Add(double.Parse(inspDataList[i][(int)CMES_BEF.AX]));
                            dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["AY"].Add(double.Parse(inspDataList[i][(int)CMES_BEF.AY]));
                            dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["BX"].Add(double.Parse(inspDataList[i][(int)CMES_BEF.BX]));
                            dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["BY"].Add(double.Parse(inspDataList[i][(int)CMES_BEF.BY]));
                            dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["C"].Add(double.Parse(inspDataList[i][(int)CMES_BEF.C]));
                            //dataDict[tokken[(int)CMES_BEF.SLOT_LAYER]]["JUMP"].Add(float.Parse(tokken[8]));

                            // FORD 라인 이면서 7-8 Layer가 아니거나
                            // P1F, J190 라인 이면서 1-2 Layer인 경우만 jump 구간 계산
                            if ((iniManagement.iniModel.Process_Line == (int)LINE.FORD && !inspDataList[i][(int)CMES_BEF.SLOT_LAYER].Contains("7/8") ||
                                 iniManagement.iniModel.Process_Line != (int)LINE.FORD && inspDataList[i][(int)CMES_BEF.SLOT_LAYER].Contains("1/2")))
                            {
                                double x = double.Parse(inspDataList[i + 1][((int)CMES_BEF.AX)]);
                                double y = double.Parse(inspDataList[i + 1][((int)CMES_BEF.AY)]);

                                double jump = UserUtil.GetR(double.Parse(inspDataList[i][(int)CMES_BEF.BX]), x, double.Parse(inspDataList[i][(int)CMES_BEF.BY]), y);
                                dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["JUMP"].Add(jump);
                            }
                            else
                            {
                                dataDict[inspDataList[i][(int)CMES_BEF.SLOT_LAYER]]["JUMP"].Add(0);
                            }

                        }
                    }
                }

                // 파일저장
                //SLOT, LAYER, 
                //AX_AVG, AX_MIN, AX_MAX, AX_ABS, 
                //AY_AVG, AY_MIN, AY_MAX, AY_ABS, 
                //BX_AVG, BX_MIN, BX_MAX, BX_ABS, 
                //BY_AVG, BY_MIN, BY_MAX, BY_ABS,
                //C_AVG, C_MIN, C_MAX, C_ABS,
                //JUMP_AVG, JUMP_MIN, JUMP_MAX, JUMP_ABS
                string dataPath = string.Empty;

                if (model == 1)
                    dataPath = Application.StartupPath + "\\ER11befPst.dat";
                else if (model == 2)
                    dataPath = Application.StartupPath + "\\EF21befPst.dat";
                else
                    dataPath = Application.StartupPath + "\\befPst.dat";

                using (StreamWriter sw = new StreamWriter(dataPath, false))
                {
                    sw.WriteLine("SLOT,LAYER,AX_AVG,AX_MIN,AX_MAX,AX_ABS,AY_AVG,AY_MIN,AY_MAX,AY_ABS,BX_AVG,BX_MIN,BX_MAX,BX_ABS,BY_AVG,BY_MIN,BY_MAX,BY_ABS,C_AVG,C_MIN,C_MAX,C_ABS,JUMP_AVG,JUMP_MIN,JUMP_MAX,JUMP_ABS");
                    foreach (KeyValuePair<string, Dictionary<string, List<double>>> slot in dataDict)
                    {
                        // Dictionary<string, Dictionary<string, List<double>>> dataDict = new Dictionary<string, Dictionary<string, List<double>>>();

                        string ax = GetData(slot.Value["AX"].Average(), slot.Value["AX"].Max(), slot.Value["AX"].Min());
                        string ay = GetData(slot.Value["AY"].Average(), slot.Value["AY"].Max(), slot.Value["AY"].Min());
                        string bx = GetData(slot.Value["BX"].Average(), slot.Value["BX"].Max(), slot.Value["BX"].Min());
                        string by = GetData(slot.Value["BY"].Average(), slot.Value["BY"].Max(), slot.Value["BY"].Min());
                        string c = GetData(slot.Value["C"].Average(), slot.Value["C"].Max(), slot.Value["C"].Min());
                        string jump = GetData(slot.Value["JUMP"].Average(), slot.Value["JUMP"].Max(), slot.Value["JUMP"].Min());

                        string[] tokken = slot.Key.Split('_');
                        string tSlot = tokken[0];
                        string tLayer = tokken[1].Replace('/', '-');

                        sw.WriteLine(string.Format("{0},{1},{2},{3},{4},{5},{6},{7}", tSlot, tLayer, ax, ay, bx, by, c, jump));
                    }
                }

                return true;
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return false;
            }
        }

        private string GetData(double avg, double max, double min)
        {
            return string.Format("{0},{1},{2},{3}",
                UserUtil.GetTruncate(avg), UserUtil.GetTruncate(min), UserUtil.GetTruncate(max), UserUtil.GetTruncate(Math.Abs(min - max)));
        }

        private void btn_LoadData_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDlg = new OpenFileDialog();            
            openFileDlg.DefaultExt = "CSV";
            openFileDlg.Filter = "CSV Files (*.csv)|*.csv";
            if (openFileDlg.ShowDialog() == DialogResult.OK)
            {
                lstPstTable.Rows.Clear();
                ProcessingPositionDerail(openFileDlg.FileName);
                gv_LstPst.RefreshData();
            }
        }

        private void ProcessingPositionDerail(string csvFile)
        {
            try
            {
                if (!File.Exists(csvFile))
                {
                    MessageBox.Show(csvFile + "이 없습니다.");
                    return;
                }

                string dataPath = string.Empty;
                if (modelNo == 1)
                    dataPath = Application.StartupPath + "\\ER11befPst.dat";
                else if (modelNo == 2)
                    dataPath = Application.StartupPath + "\\EF21befPst.dat";
                else
                    dataPath = Application.StartupPath + "\\befPst.dat";

                string copyFile = Application.StartupPath + "\\pBefPst.dat";
                File.Copy(dataPath, copyFile);

                bool isFirst = true;
                // 전검사 파일 읽기
                List<string[]> befCmesList = new List<string[]>();
                using (StreamReader sr = new StreamReader(csvFile, Encoding.UTF8))
                {
                    while (!sr.EndOfStream)
                    {
                        string[] tokken = sr.ReadLine().Split(',');
                        if (isFirst)
                        {
                            isFirst = false;
                            continue;
                        }

                        befCmesList.Add(tokken);
                    }
                }

                // 기준 파일 읽기
                isFirst = true;
                int index = 0;
                double jump = 0;
                using (StreamReader sr = new StreamReader(copyFile, Encoding.UTF8))
                {
                    while (!sr.EndOfStream)
                    {
                        string[] tokken = sr.ReadLine().Split(',');
                        if (isFirst)
                        {
                            isFirst = false;
                            continue;
                        }

                        // SLOT, LAYER, 1H, 2H, 3H, 4H, AX, AY, INNER_R, BX, BY, OUTER_R, C, JUMP
                        DataRow row = lstPstTable.NewRow();
                        row[0] = tokken[(int)BEF_POINT.SLOT];
                        row[1] = tokken[(int)BEF_POINT.LAYER];
                        row[2] = befCmesList[index][(int)CMES_BEF.H1];
                        row[3] = befCmesList[index][(int)CMES_BEF.H2];
                        row[4] = befCmesList[index][(int)CMES_BEF.H3];
                        row[5] = befCmesList[index][(int)CMES_BEF.H4];
                        row[6] = befCmesList[index][(int)CMES_BEF.AX];
                        row[7] = befCmesList[index][(int)CMES_BEF.AY];
                        row[8] = UserUtil.GetR(double.Parse(tokken[(int)BEF_POINT.AX_AVG]), double.Parse(befCmesList[index][(int)CMES_BEF.AX]), 
                            double.Parse(tokken[(int)BEF_POINT.AY_AVG]), double.Parse(befCmesList[index][(int)CMES_BEF.AY]));
                        row[9] = befCmesList[index][(int)CMES_BEF.BX];
                        row[10] = befCmesList[index][(int)CMES_BEF.BY];
                        row[11] = UserUtil.GetR(double.Parse(tokken[(int)BEF_POINT.BX_AVG]), double.Parse(befCmesList[index][(int)CMES_BEF.BX]), 
                            double.Parse(tokken[(int)BEF_POINT.BY_AVG]), double.Parse(befCmesList[index][(int)CMES_BEF.BY]));
                        row[12] = befCmesList[index][(int)CMES_BEF.C];

                        // FORD 라인 이면서 7-8 Layer가 아니거나
                        // P1F, J190 라인 이면서 1-2 Layer인 경우만 jump 구간 계산
                        if ((iniManagement.iniModel.Process_Line == (int)LINE.FORD && tokken[(int)BEF_POINT.LAYER] != "7-8") ||
                                iniManagement.iniModel.Process_Line != (int)LINE.FORD && tokken[(int)BEF_POINT.LAYER] == "1-2")
                        {
                            double x = double.Parse(befCmesList[index + 1][(int)CMES_BEF.AX]) / 100;
                            double y = double.Parse(befCmesList[index + 1][(int)CMES_BEF.AY]) / 100;

                            jump = UserUtil.GetR(double.Parse(befCmesList[index][(int)CMES_BEF.BX]), x, double.Parse(befCmesList[index][(int)CMES_BEF.BY]), y);  
                        }
                        else
                        {
                            jump = -1;
                        }

                        row[13] = jump.ToString();
                        lstPstTable.Rows.Add(row);

                        index++;
                    }
                }

                File.Delete(copyFile);
            }
            catch(Exception e)
            {
                MessageBox.Show(e.Message);
            }
        }

        private void gv_CellMerge(object sender, CellMergeEventArgs e)
        {
            CellMerge(gv_BefPst, e);
        }

        private void gv_LstPst_CellMerge(object sender, CellMergeEventArgs e)
        {
            CellMerge(gv_LstPst, e);
        }

        private void CellMerge(GridView gv, CellMergeEventArgs e)
        {
            try
            {
                if (e.Column.FieldName == "Slot")
                {
                    var dr1 = gv.GetDataRow(e.RowHandle1); //위에 행 정보
                    var dr2 = gv.GetDataRow(e.RowHandle2); //아래 행 정보

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
                
            }
        }

        private void gv_RowCellStyle(object sender, RowCellStyleEventArgs e)
        {
            try
            {
                if (e.Column.FieldName == "Slot" || e.Column.FieldName == "Layer")
                {
                    e.Appearance.BackColor = Color.FromArgb(105, 105, 105);
                    e.Appearance.ForeColor = Color.White;
                }
            }
            catch { }
        }

        private void gv_BefPst_CustomDrawCell(object sender, DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventArgs e)
        {
            try
            {
                BandedGridColumn column = (BandedGridColumn)e.Column;
                if (column.OwnerBand.Caption == " ")
                {
                    e.Appearance.BackColor = Color.DimGray;
                    e.Appearance.ForeColor = Color.White;
                }
                else if (column.OwnerBand.Caption == "AY" || column.OwnerBand.Caption == "BY" || column.OwnerBand.Caption == "JUMP")
                {
                    e.Appearance.BackColor = Color.LightSteelBlue;
                }
            }
            catch { }

        }

        private void gv_LstPst_CustomDrawCell(object sender, DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventArgs e)
        {

            try
            {
                if (e.Column.FieldName == "AR" || e.Column.FieldName == "BR" || e.Column.FieldName == "JUMP")
                {
                    e.Appearance.BackColor = Color.LightSteelBlue;
                    cellValue = double.Parse(e.CellValue.ToString());
                    if (e.Column.FieldName == "AR" || e.Column.FieldName == "BR")
                    {

                        if (cellValue > iniManagement.iniModel.Verify_RThreshold)
                        {
                            e.Appearance.BackColor = Color.Red;
                            e.Appearance.ForeColor = Color.White;
                        }
                    }
                    else
                    {
                        if (!(cellValue > iniManagement.iniModel.Verify_JumpMin && cellValue < iniManagement.iniModel.Verify_JumpMax) && cellValue != -1)
                        {
                            e.Appearance.BackColor = Color.Red;
                            e.Appearance.ForeColor = Color.White;
                        }
                    }
                }
            }
            catch
            {

            }

        }


    }
}
