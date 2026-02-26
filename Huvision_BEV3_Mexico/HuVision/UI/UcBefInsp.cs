using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using DevExpress.XtraGrid.Views.Grid;
using HuVision.Model;

namespace HuVision.UI
{
    public partial class UcBefInsp : UserControl
    {
        private DataTable dataTable = new DataTable();
        private IniManagement iniManagement;

        private double D2R = 0.0174532925199433;
        private Font drawFont = new Font("Consolas", 9, FontStyle.Bold);
        private SolidBrush okWBrush = new SolidBrush(Color.White);
        private SolidBrush okBBrush = new SolidBrush(Color.Black);
        private SolidBrush ngBrush = new SolidBrush(Color.Red);

        private short[] drawResultArr;          // Slot Drawing 데이터 

        private string befLotID;
        private int productCount;

        public UcBefInsp()
        {
            InitializeComponent();

            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.DoubleBuffer, true);

            iniManagement = IniManagement.Instance();

            drawResultArr = new short[iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots];
            for (int i = 0; i < drawResultArr.Length; i++)
                drawResultArr[i] = (int)RESPONSE.OK;

            gridControl.DataSource = dataTable;

            InitGridControl();
        }

        private void UcBefInsp_Load(object sender, EventArgs e)
        {
            if (File.Exists("BefImg.png"))
                pb_Image.Image = Image.FromFile("BefImg.png");
        }

        private void InitGridControl()
        {
            CreateColumn("Slot", "Slot", 20, dataTable, gridView, typeof(int));
            CreateColumn("Layer", "Layer", 30, dataTable, gridView, typeof(string));

            CreateColumn("1H", "1H", 30, dataTable, gridView, typeof(double));
            CreateColumn("2H", "2H", 30, dataTable, gridView, typeof(double));
            CreateColumn("3H", "3H", 30, dataTable, gridView, typeof(double));
            CreateColumn("4H", "4H", 30, dataTable, gridView, typeof(double));
            CreateColumn("C", "C", 30, dataTable, gridView, typeof(double));
            CreateColumn("Inner R", "Inner R", 30, dataTable, gridView, typeof(double));
            CreateColumn("Outer R", "Outer R", 30, dataTable, gridView, typeof(double));
            CreateColumn("Jump", "Jump", 30, dataTable, gridView, typeof(double));
            CreateColumn("AX", "AX", 0, dataTable, gridView, typeof(double));
            CreateColumn("AY", "AY", 0, dataTable, gridView, typeof(double));
            CreateColumn("BX", "BX", 0, dataTable, gridView, typeof(double));
            CreateColumn("BY", "BY", 0, dataTable, gridView, typeof(double));
            CreateColumn("Result", "Result", 0, dataTable, gridView, typeof(int));
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
                LogManager.WriteLog(LogType.Error, e);
            }
        }

        public void SetBefInsp(List<BefInspModel> befInspModelList, string lotID)
        {
            try
            {
                dataTable.Rows.Clear();
                int okCount = 0;
                int ngCount = 0;
                DateTime time = DateTime.Now;

                string csvForderPath = string.Format("{0}\\Before\\{1}\\{2}\\{3}", iniManagement.iniModel.Process_SavePath, time.ToString("yyyy"), time.ToString("MM"), time.ToString("dd"));
                if (!Directory.Exists(csvForderPath))
                    Directory.CreateDirectory(csvForderPath);

                string saveCsvPath = string.Format("{0}\\{1}_{2}.csv", csvForderPath, time.ToString("HH_mm_ss"), lotID);
                LogManager.WriteLog(LogType.Information, "전검사: " + saveCsvPath);
                
                using (StreamWriter sw = new StreamWriter(saveCsvPath, false, Encoding.UTF8))
                {
                    sw.WriteLine("Slot,Layer,1H,2H,3H,4H,C,InnerR,OuterR,Jump,AX,AY,BX,BY,Result");

                    foreach (BefInspModel model in befInspModelList)
                    {
                        DataRow row = dataTable.NewRow();
                        row[0] = model.slot;
                        row[1] = model.layer;
                        row[2] = model.H1;
                        row[3] = model.H2;
                        row[4] = model.H3;
                        row[5] = model.H4;
                        row[6] = model.c;
                        row[7] = model.innerR;
                        row[8] = model.outerR;
                        row[9] = model.jump;
                        row[10] = model.aX;
                        row[11] = model.aY;
                        row[12] = model.bX;
                        row[13] = model.bY;
                        row[14] = model.result;

                        dataTable.Rows.Add(row);

                        sw.WriteLine("{0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14}",
                            model.slot, model.layer, model.H1, model.H2, model.H3, model.H4, model.c, model.innerR, model.outerR, 
                            model.jump, model.aX, model.aY, model.bX, model.bY, model.result);

                        if (model.result == 1)
                            okCount++;
                        else
                            ngCount++;
                    }
                }

                gridView.RefreshData();

                lbl_Result.Text = string.Format("OK: {0} NG: {1} ", okCount.ToString("D2"), ngCount.ToString("D2"));
                if (ngCount > 0)
                    lbl_Result.ForeColor = Color.Red;
                else
                    lbl_Result.ForeColor = Color.Green;

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

                ProcessingDrawResult(befInspModelList);
            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, "SetBefInsp: " + e.Message);
            }
        }

        private void ProcessingDrawResult(List<BefInspModel> befInspModelList)
        {
            try
            {
                drawResultArr = new short[iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots];
                int index = 0;
                int error = 0;
                int spaceCount = (iniManagement.inspectionModel.WeldAfterInspect_NumberofLayers / 2);

                for (int i = 0; i < befInspModelList.Count; i += spaceCount)
                {
                    error = 0;
                    for (int j = 0; j < spaceCount; j++)
                        error += befInspModelList[i + j].result;

                    if (error > spaceCount)
                        drawResultArr[index++] = (int)RESPONSE.NG;
                    else
                        drawResultArr[index++] = (int)RESPONSE.OK;
                }

                Draw();
            }
            catch (Exception e)
            {
                
            }
        }

        private void pb_Image_Paint(object sender, PaintEventArgs e)
        {
            Draw(e.Graphics);
        }

        private void Draw(Graphics g = null)
        {
            try
            {
                if (g == null)
                    g = pb_Image.CreateGraphics();

                double angle = 360.0 / iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots;
                double x, y;

                float centerX = pb_Image.Width / 2;
                float centerY = pb_Image.Height / 2;

                for (int i = 0; i < iniManagement.inspectionModel.WeldAfterInspect_NumberofSlots; i++)
                {
                    double radius = iniManagement.iniModel.Draw_BefRange / 2.0;
                    double dA = 90 + i * angle + iniManagement.iniModel.Draw_BefAngle;

                    UserUtil.P2R(radius - 4 / 2, dA * D2R, out x, out y);

                    if (iniManagement.iniModel.Draw_IsBlack)
                    {
                        g.DrawString((i + 1).ToString(), drawFont, drawResultArr[i] == (int)RESPONSE.OK ? okBBrush : ngBrush,
                        (float)x + centerX + iniManagement.iniModel.Draw_BefXOffSet, -(float)y + centerY + iniManagement.iniModel.Draw_BefYOffSet);
                    }
                    else
                    {
                        g.DrawString((i + 1).ToString(), drawFont, drawResultArr[i] == (int)RESPONSE.OK ? okWBrush : ngBrush,
                        (float)x + centerX + iniManagement.iniModel.Draw_BefXOffSet, -(float)y + centerY + iniManagement.iniModel.Draw_BefYOffSet);
                    }
                }

                //g.Dispose();
            }
            catch { }
        }

        private void UcBefInsp_Resize(object sender, EventArgs e)
        {
            gridView.RefreshData();
        }

        private void gridView_CellMerge(object sender, CellMergeEventArgs e)
        {
            try
            {
                if (e.Column.FieldName == "Slot")
                {
                    var dr1 = gridView.GetDataRow(e.RowHandle1); //위에 행 정보
                    var dr2 = gridView.GetDataRow(e.RowHandle2); //아래 행 정보

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

        private void gridView_CustomDrawCell(object sender, DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventArgs e)
        {
            try
            {
                //GridView view = sender as GridView;
                //if (e.RowHandle == view.FocusedRowHandle) return;

                if (e.Column.FieldName == "1H" || e.Column.FieldName == "2H" || e.Column.FieldName == "3H" || e.Column.FieldName == "4H" || e.Column.FieldName == "C")
                    e.Appearance.ForeColor = Color.Blue;

                if(e.Column.FieldName != "Slot" && e.Column.FieldName != "Layer" && e.Column.FieldName != "Result")
                {
                    
                    if(e.Column.FieldName == "3H")
                    {
                        if(e.CellValue.ToString() == "0")
                        {
                            string h1 = gridView.GetRowCellDisplayText(e.RowHandle, gridView.Columns["1H"]);
                            string h2 = gridView.GetRowCellDisplayText(e.RowHandle, gridView.Columns["2H"]);
                            if (h1 == h2)
                                return;
                        }
                        else { return; }
                    }
                    else if(e.Column.FieldName == "Inner R" || e.Column.FieldName == "Outer R")
                    {
                        if (double.Parse(e.CellValue.ToString()) < iniManagement.iniModel.Verify_RThreshold)
                            return;
                    }
                    else
                    {
                        if (e.CellValue.ToString() != "0")
                            return;
                    }

                    e.Appearance.BackColor = Color.Red;
                    e.Appearance.ForeColor = Color.White;
                }
                
                //string result = gridView.GetRowCellDisplayText(e.RowHandle, gridView.Columns["Result"]);
                //if (result == "2" || result == "3")
                //{
                //    e.Appearance.BackColor = Color.Red;
                //    e.Appearance.ForeColor = Color.White;
                //}
            }
            catch { }
        }

        private void gridView_RowCellStyle(object sender, RowCellStyleEventArgs e)
        {
            try
            {
                int slot = int.Parse(gridView.GetRowCellDisplayText(e.RowHandle, gridView.Columns["Slot"]));
                if (slot % 2 == 0)
                {
                    e.Appearance.BackColor = Color.FromArgb(235, 235, 235);
                    //e.Appearance.ForeColor = Color.White;
                }
                //if (e.Column.FieldName == "Slot" || e.Column.FieldName == "Layer")
                //{
                //    e.Appearance.BackColor = Color.FromArgb(105, 105, 105);
                //    e.Appearance.ForeColor = Color.White;
                //}
            }
            catch { }
        }

        private void panel1_Paint(object sender, PaintEventArgs e)
        {
            Draw(e.Graphics);
        }

        private void pb_Open_Click(object sender, EventArgs e)
        {
            try
            {
                int okCount = 0;
                int ngCount = 0;

                OpenFileDialog openFileDlg = new OpenFileDialog();
                openFileDlg.DefaultExt = "CSV";
                openFileDlg.Filter = "CSV Files (*.csv)|*.csv";
                if (openFileDlg.ShowDialog() == DialogResult.OK)
                {
                    lbl_LotID.Text = string.Format("LotID: {0}     Count: {1}", "", 0);

                    dataTable.Rows.Clear();
                    bool isFirst = true;
                    List<BefInspModel> list = new List<BefInspModel>();
                    using(StreamReader sr = new StreamReader(openFileDlg.FileName, Encoding.UTF8))
                    {
                        while(!sr.EndOfStream)
                        {
                            string[] tokken = sr.ReadLine().Split(',');
                            if (isFirst)
                            {
                                isFirst = false;
                                continue;
                            }

                            // Slot,Layer,1H,2H,3H,4H,C,InnerR,OuterR,Jump,AX,AY,BX,BY,Result
                            DataRow row = dataTable.NewRow();
                            row.ItemArray = tokken.Cast<object>().ToArray();
                            dataTable.Rows.Add(row);

                            BefInspModel model = new BefInspModel();
                            //model.slot = int.Parse(tokken[0]);
                            //model.layer = tokken[1];
                            //model.H1 = double.Parse(tokken[2]);
                            //model.H2 = double.Parse(tokken[3]);
                            //model.H3 = double.Parse(tokken[4]);
                            //model.H4 = double.Parse(tokken[5]);
                            //model.c = double.Parse(tokken[6]);
                            //model.innerR = double.Parse(tokken[7]);
                            //model.outerR = double.Parse(tokken[8]);
                            //model.jump = double.Parse(tokken[9]);
                            //model.aX = double.Parse(tokken[10]);
                            //model.aY = double.Parse(tokken[11]);
                            //model.bX = double.Parse(tokken[12]);
                            //model.bY = double.Parse(tokken[13]);
                            model.result = int.Parse(tokken[14]);
                            list.Add(model);

                            if (model.result == (int)RESPONSE.OK)
                                okCount++;
                            else
                                ngCount++;
                        }
                    }

                    lbl_Result.Text = string.Format("OK: {0} NG: {1} ", okCount.ToString("D2"), ngCount.ToString("D2"));
                    if (ngCount > 0)
                        lbl_Result.ForeColor = Color.Red;
                    else
                        lbl_Result.ForeColor = Color.Green;


                    ProcessingDrawResult(list);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void pb_Refresh_Click(object sender, EventArgs e)
        {
            gridView.ClearSorting();
            //BefInspModelRefresh();
        }

        private void cb_Position_CheckedChanged(object sender, EventArgs e)
        {
            for (int i = 10; i < 14; i++ )
            {
                gridView.Columns[i].Visible = cb_Position.Checked;
                gridView.Columns[i].Width = 70;
            }

            gridControl.Refresh();
        }

        public void GridViewRefreshData()
        {
            gridView.RefreshData();
        }
    }
}
