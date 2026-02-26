namespace HuVision.UI
{
    partial class UcInspection2
    {
        /// <summary> 
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 구성 요소 디자이너에서 생성한 코드

        /// <summary> 
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UcInspection2));
            this.tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.tbx_Log = new System.Windows.Forms.TextBox();
            this.pnl_Top = new System.Windows.Forms.Panel();
            this.lbl_LotID = new System.Windows.Forms.Label();
            this.lbl_Result = new System.Windows.Forms.Label();
            this.pb_Open = new System.Windows.Forms.PictureBox();
            this.gridControl = new DevExpress.XtraGrid.GridControl();
            this.gridView = new DevExpress.XtraGrid.Views.Grid.GridView();
            this.pnl_Image = new System.Windows.Forms.Panel();
            this.tableLayoutPanel.SuspendLayout();
            this.pnl_Top.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Open)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridControl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridView)).BeginInit();
            this.SuspendLayout();
            // 
            // tableLayoutPanel
            // 
            this.tableLayoutPanel.ColumnCount = 3;
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 700F));
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel.Controls.Add(this.tbx_Log, 0, 4);
            this.tableLayoutPanel.Controls.Add(this.pnl_Top, 0, 0);
            this.tableLayoutPanel.Controls.Add(this.gridControl, 1, 1);
            this.tableLayoutPanel.Controls.Add(this.pnl_Image, 0, 1);
            this.tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel.Name = "tableLayoutPanel";
            this.tableLayoutPanel.Padding = new System.Windows.Forms.Padding(5);
            this.tableLayoutPanel.RowCount = 5;
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 53F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 150F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 101F));
            this.tableLayoutPanel.Size = new System.Drawing.Size(1381, 488);
            this.tableLayoutPanel.TabIndex = 0;
            // 
            // tbx_Log
            // 
            this.tbx_Log.BackColor = System.Drawing.Color.White;
            this.tableLayoutPanel.SetColumnSpan(this.tbx_Log, 3);
            this.tbx_Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbx_Log.Font = new System.Drawing.Font("Consolas", 9F);
            this.tbx_Log.ForeColor = System.Drawing.Color.Black;
            this.tbx_Log.Location = new System.Drawing.Point(8, 385);
            this.tbx_Log.Multiline = true;
            this.tbx_Log.Name = "tbx_Log";
            this.tbx_Log.ReadOnly = true;
            this.tbx_Log.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tbx_Log.Size = new System.Drawing.Size(1365, 95);
            this.tbx_Log.TabIndex = 31;
            // 
            // pnl_Top
            // 
            this.tableLayoutPanel.SetColumnSpan(this.pnl_Top, 3);
            this.pnl_Top.Controls.Add(this.lbl_LotID);
            this.pnl_Top.Controls.Add(this.lbl_Result);
            this.pnl_Top.Controls.Add(this.pb_Open);
            this.pnl_Top.Location = new System.Drawing.Point(8, 8);
            this.pnl_Top.Name = "pnl_Top";
            this.pnl_Top.Size = new System.Drawing.Size(1365, 47);
            this.pnl_Top.TabIndex = 63;
            // 
            // lbl_LotID
            // 
            this.lbl_LotID.AutoSize = true;
            this.lbl_LotID.Font = new System.Drawing.Font("Consolas", 20F, System.Drawing.FontStyle.Bold);
            this.lbl_LotID.ForeColor = System.Drawing.Color.Black;
            this.lbl_LotID.Location = new System.Drawing.Point(628, 8);
            this.lbl_LotID.Name = "lbl_LotID";
            this.lbl_LotID.Size = new System.Drawing.Size(285, 32);
            this.lbl_LotID.TabIndex = 7;
            this.lbl_LotID.Text = "LotID:     Count: ";
            // 
            // lbl_Result
            // 
            this.lbl_Result.AutoSize = true;
            this.lbl_Result.Font = new System.Drawing.Font("Consolas", 20F, System.Drawing.FontStyle.Bold);
            this.lbl_Result.ForeColor = System.Drawing.Color.Black;
            this.lbl_Result.Location = new System.Drawing.Point(51, 8);
            this.lbl_Result.Name = "lbl_Result";
            this.lbl_Result.Size = new System.Drawing.Size(435, 32);
            this.lbl_Result.TabIndex = 6;
            this.lbl_Result.Text = "PASS: 00 REWORK: 00 FAIL: 00";
            // 
            // pb_Open
            // 
            this.pb_Open.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pb_Open.Image = ((System.Drawing.Image)(resources.GetObject("pb_Open.Image")));
            this.pb_Open.Location = new System.Drawing.Point(4, 4);
            this.pb_Open.Name = "pb_Open";
            this.pb_Open.Size = new System.Drawing.Size(40, 40);
            this.pb_Open.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pb_Open.TabIndex = 0;
            this.pb_Open.TabStop = false;
            this.pb_Open.Click += new System.EventHandler(this.pb_Open_Click);
            // 
            // gridControl
            // 
            this.tableLayoutPanel.SetColumnSpan(this.gridControl, 2);
            this.gridControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridControl.Location = new System.Drawing.Point(708, 61);
            this.gridControl.MainView = this.gridView;
            this.gridControl.Name = "gridControl";
            this.tableLayoutPanel.SetRowSpan(this.gridControl, 3);
            this.gridControl.Size = new System.Drawing.Size(665, 318);
            this.gridControl.TabIndex = 65;
            this.gridControl.ViewCollection.AddRange(new DevExpress.XtraGrid.Views.Base.BaseView[] {
            this.gridView});
            // 
            // gridView
            // 
            this.gridView.Appearance.HeaderPanel.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gridView.Appearance.HeaderPanel.Options.UseFont = true;
            this.gridView.Appearance.HeaderPanel.Options.UseTextOptions = true;
            this.gridView.Appearance.HeaderPanel.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gridView.Appearance.Row.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gridView.Appearance.Row.Options.UseFont = true;
            this.gridView.Appearance.Row.Options.UseTextOptions = true;
            this.gridView.Appearance.Row.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gridView.GridControl = this.gridControl;
            this.gridView.Name = "gridView";
            this.gridView.OptionsBehavior.Editable = false;
            this.gridView.OptionsCustomization.AllowColumnMoving = false;
            this.gridView.OptionsCustomization.AllowColumnResizing = false;
            this.gridView.OptionsCustomization.AllowFilter = false;
            this.gridView.OptionsCustomization.AllowGroup = false;
            this.gridView.OptionsCustomization.AllowSort = false;
            this.gridView.OptionsSelection.MultiSelect = true;
            this.gridView.OptionsView.AllowCellMerge = true;
            this.gridView.OptionsView.ShowGroupPanel = false;
            this.gridView.CellMerge += new DevExpress.XtraGrid.Views.Grid.CellMergeEventHandler(this.gridView_CellMerge);
            this.gridView.CustomDrawCell += new DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventHandler(this.gridView_CustomDrawCell);
            this.gridView.RowCellStyle += new DevExpress.XtraGrid.Views.Grid.RowCellStyleEventHandler(this.gridView_RowCellStyle);
            this.gridView.RowStyle += new DevExpress.XtraGrid.Views.Grid.RowStyleEventHandler(this.gridView_RowStyle);
            // 
            // pnl_Image
            // 
            this.pnl_Image.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.pnl_Image.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnl_Image.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnl_Image.Location = new System.Drawing.Point(8, 61);
            this.pnl_Image.Name = "pnl_Image";
            this.tableLayoutPanel.SetRowSpan(this.pnl_Image, 3);
            this.pnl_Image.Size = new System.Drawing.Size(694, 318);
            this.pnl_Image.TabIndex = 66;
            this.pnl_Image.Paint += new System.Windows.Forms.PaintEventHandler(this.pnl_Image_Paint);
            // 
            // UcInspection2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.tableLayoutPanel);
            this.Name = "UcInspection2";
            this.Size = new System.Drawing.Size(1381, 488);
            this.Resize += new System.EventHandler(this.UcInspection_Resize);
            this.tableLayoutPanel.ResumeLayout(false);
            this.tableLayoutPanel.PerformLayout();
            this.pnl_Top.ResumeLayout(false);
            this.pnl_Top.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Open)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridControl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
        private System.Windows.Forms.TextBox tbx_Log;
        private System.Windows.Forms.Panel pnl_Top;
        private System.Windows.Forms.PictureBox pb_Open;
        private System.Windows.Forms.Label lbl_Result;
        private System.Windows.Forms.Label lbl_LotID;
        private DevExpress.XtraGrid.GridControl gridControl;
        private DevExpress.XtraGrid.Views.Grid.GridView gridView;
        private System.Windows.Forms.Panel pnl_Image;
    }
}
