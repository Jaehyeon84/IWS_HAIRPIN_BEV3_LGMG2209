using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using HuVision.Model;
using DevExpress.XtraGrid.Views.Grid;

namespace HuVision.UI
{
    public partial class UcBefData : UserControl
    {
        private DataTable dataTable = new DataTable();
        private IniModel iniModel = new IniModel();

        private enum COL { Slot = 0, Pin, L1H, L2H, L3H, L4H, aX, aY, bX, bY, C, θ };

        public UcBefData()
        {
            InitializeComponent();

            new IniManagement().OpenIni(iniModel, Application.StartupPath + "\\WeldAfterInspect.ini");

            InitGridControl();
            gridControl.DataSource = dataTable;
        }

        private void InitGridControl()
        {
            CreateColumn("Slot", "Slot", 50, dataTable, gridView, typeof(int));
            CreateColumn("Pin", "Pin", 30, dataTable, gridView, typeof(string));

            CreateColumn("1H", "1H", 50, dataTable, gridView, typeof(double));
            CreateColumn("2H", "2H", 50, dataTable, gridView, typeof(double));
            CreateColumn("3H", "3H", 50, dataTable, gridView, typeof(double));
            CreateColumn("4H", "4H", 50, dataTable, gridView, typeof(double));

            CreateColumn("aX", "aX", 50, dataTable, gridView, typeof(double));
            CreateColumn("aY", "aY", 50, dataTable, gridView, typeof(double));
            CreateColumn("bX", "bX", 50, dataTable, gridView, typeof(double));
            CreateColumn("bY", "bY", 50, dataTable, gridView, typeof(double));
            CreateColumn("C", "C", 50, dataTable, gridView, typeof(double));
            CreateColumn("θ", "θ", 50, dataTable, gridView, typeof(double));          

            for (int i = 1; i < iniModel.WeldAfterInspect_NumberofSlots + 1; i++)
            {
                for (int j = 0; j < (iniModel.WeldAfterInspect_NumberofLayers / 2); j++)
                {
                    DataRow row = dataTable.NewRow();

                    row[(int)COL.Slot] = i;
                    switch (j)
                    {
                        case 0: row[(int)COL.Pin] = "1-2"; break;
                        case 1: row[(int)COL.Pin] = "3-4"; break;
                        case 2: row[(int)COL.Pin] = "5-6"; break;
                        case 3: row[(int)COL.Pin] = "7-8"; break;
                        case 4: row[(int)COL.Pin] = "9-10"; break;
                        case 5: row[(int)COL.Pin] = "11-12"; break;
                    }

                    for (int k = (int)COL.L1H; k < (int)COL.θ + 1; k++)
                        row[k] = 0.0;

                    dataTable.Rows.Add(row);
                }
            }
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

        public void SetInspectionData(List<string> l1H, List<string> l2H, List<string> l3H, List<string> l4H,
            List<string> aX, List<string> aY, List<string> bX, List<string> bY, List<string> cd, List<string> tt)
        {
            try
            {
                for(int i = 0; i < l1H.Count; i++)
                {
                    dataTable.Rows[i][(int)COL.L1H] = double.Parse(l1H[i]) / 100d;
                    dataTable.Rows[i][(int)COL.L2H] = double.Parse(l2H[i]) / 100d;
                    dataTable.Rows[i][(int)COL.L3H] = double.Parse(l3H[i]) / 100d;
                    dataTable.Rows[i][(int)COL.L4H] = double.Parse(l4H[i]) / 100d;

                    dataTable.Rows[i][(int)COL.aX] = double.Parse(aX[i]) / 100d;
                    dataTable.Rows[i][(int)COL.aY] = double.Parse(aY[i]) / 100d;
                    dataTable.Rows[i][(int)COL.bX] = double.Parse(bX[i]) / 100d;
                    dataTable.Rows[i][(int)COL.bY] = double.Parse(bY[i]) / 100d;
                    dataTable.Rows[i][(int)COL.C] = double.Parse(cd[i]) / 100d;
                    dataTable.Rows[i][(int)COL.θ] = double.Parse(tt[i]) / 100d;
                }
            }
            catch(Exception e)
            {
                LogManager.WriteLog(LogType.Error, e);
            }
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
                
            }
        }
    }
}
