namespace HuVision.UI
{
    partial class UcBefInsp
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(UcBefInsp));
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.gridControl = new DevExpress.XtraGrid.GridControl();
            this.gridView = new DevExpress.XtraGrid.Views.Grid.GridView();
            this.pnl_Top = new System.Windows.Forms.Panel();
            this.cb_Position = new System.Windows.Forms.CheckBox();
            this.pb_Refresh = new System.Windows.Forms.PictureBox();
            this.pb_Open = new System.Windows.Forms.PictureBox();
            this.lbl_LotID = new System.Windows.Forms.Label();
            this.lbl_Result = new System.Windows.Forms.Label();
            this.pb_Image = new System.Windows.Forms.PictureBox();
            this.tableLayoutPanel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gridControl)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridView)).BeginInit();
            this.pnl_Top.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Refresh)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Open)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Image)).BeginInit();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 700F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.gridControl, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.pnl_Top, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.pb_Image, 0, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.Padding = new System.Windows.Forms.Padding(5);
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 53F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(1281, 607);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // gridControl
            // 
            this.gridControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gridControl.Location = new System.Drawing.Point(708, 61);
            this.gridControl.LookAndFeel.UseDefaultLookAndFeel = false;
            this.gridControl.MainView = this.gridView;
            this.gridControl.Name = "gridControl";
            this.gridControl.Size = new System.Drawing.Size(565, 538);
            this.gridControl.TabIndex = 66;
            this.gridControl.ViewCollection.AddRange(new DevExpress.XtraGrid.Views.Base.BaseView[] {
            this.gridView});
            // 
            // gridView
            // 
            this.gridView.Appearance.HeaderPanel.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gridView.Appearance.HeaderPanel.Options.UseFont = true;
            this.gridView.Appearance.HeaderPanel.Options.UseTextOptions = true;
            this.gridView.Appearance.HeaderPanel.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gridView.Appearance.Row.Font = new System.Drawing.Font("Consolas", 10.5F);
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
            this.gridView.OptionsCustomization.AllowRowSizing = true;
            this.gridView.OptionsSelection.MultiSelect = true;
            this.gridView.OptionsView.AllowCellMerge = true;
            this.gridView.OptionsView.ShowGroupPanel = false;
            this.gridView.RowHeight = 30;
            this.gridView.CellMerge += new DevExpress.XtraGrid.Views.Grid.CellMergeEventHandler(this.gridView_CellMerge);
            this.gridView.CustomDrawCell += new DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventHandler(this.gridView_CustomDrawCell);
            this.gridView.RowCellStyle += new DevExpress.XtraGrid.Views.Grid.RowCellStyleEventHandler(this.gridView_RowCellStyle);
            // 
            // pnl_Top
            // 
            this.tableLayoutPanel1.SetColumnSpan(this.pnl_Top, 2);
            this.pnl_Top.Controls.Add(this.cb_Position);
            this.pnl_Top.Controls.Add(this.pb_Refresh);
            this.pnl_Top.Controls.Add(this.pb_Open);
            this.pnl_Top.Controls.Add(this.lbl_LotID);
            this.pnl_Top.Controls.Add(this.lbl_Result);
            this.pnl_Top.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnl_Top.Location = new System.Drawing.Point(8, 8);
            this.pnl_Top.Name = "pnl_Top";
            this.pnl_Top.Size = new System.Drawing.Size(1265, 47);
            this.pnl_Top.TabIndex = 64;
            // 
            // cb_Position
            // 
            this.cb_Position.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cb_Position.AutoSize = true;
            this.cb_Position.Location = new System.Drawing.Point(1147, 26);
            this.cb_Position.Name = "cb_Position";
            this.cb_Position.Size = new System.Drawing.Size(69, 16);
            this.cb_Position.TabIndex = 10;
            this.cb_Position.Text = "Position";
            this.cb_Position.UseVisualStyleBackColor = true;
            this.cb_Position.CheckedChanged += new System.EventHandler(this.cb_Position_CheckedChanged);
            // 
            // pb_Refresh
            // 
            this.pb_Refresh.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pb_Refresh.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pb_Refresh.Image = global::HuVision.Properties.Resources.iconfinder_icon_refresh_211882;
            this.pb_Refresh.Location = new System.Drawing.Point(1220, 4);
            this.pb_Refresh.Name = "pb_Refresh";
            this.pb_Refresh.Size = new System.Drawing.Size(40, 40);
            this.pb_Refresh.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pb_Refresh.TabIndex = 9;
            this.pb_Refresh.TabStop = false;
            this.pb_Refresh.Click += new System.EventHandler(this.pb_Refresh_Click);
            // 
            // pb_Open
            // 
            this.pb_Open.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pb_Open.Image = ((System.Drawing.Image)(resources.GetObject("pb_Open.Image")));
            this.pb_Open.Location = new System.Drawing.Point(5, 4);
            this.pb_Open.Name = "pb_Open";
            this.pb_Open.Size = new System.Drawing.Size(40, 40);
            this.pb_Open.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pb_Open.TabIndex = 8;
            this.pb_Open.TabStop = false;
            this.pb_Open.Click += new System.EventHandler(this.pb_Open_Click);
            // 
            // lbl_LotID
            // 
            this.lbl_LotID.AutoSize = true;
            this.lbl_LotID.Font = new System.Drawing.Font("Consolas", 20F, System.Drawing.FontStyle.Bold);
            this.lbl_LotID.ForeColor = System.Drawing.Color.Black;
            this.lbl_LotID.Location = new System.Drawing.Point(627, 8);
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
            this.lbl_Result.Location = new System.Drawing.Point(52, 8);
            this.lbl_Result.Name = "lbl_Result";
            this.lbl_Result.Size = new System.Drawing.Size(210, 32);
            this.lbl_Result.TabIndex = 6;
            this.lbl_Result.Text = "OK: 00 NG: 00";
            // 
            // pb_Image
            // 
            this.pb_Image.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pb_Image.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pb_Image.Location = new System.Drawing.Point(8, 61);
            this.pb_Image.Name = "pb_Image";
            this.pb_Image.Size = new System.Drawing.Size(694, 538);
            this.pb_Image.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pb_Image.TabIndex = 67;
            this.pb_Image.TabStop = false;
            this.pb_Image.Paint += new System.Windows.Forms.PaintEventHandler(this.pb_Image_Paint);
            // 
            // UcBefInsp
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.tableLayoutPanel1);
            this.Name = "UcBefInsp";
            this.Size = new System.Drawing.Size(1281, 607);
            this.Load += new System.EventHandler(this.UcBefInsp_Load);
            this.Resize += new System.EventHandler(this.UcBefInsp_Resize);
            this.tableLayoutPanel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gridControl)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gridView)).EndInit();
            this.pnl_Top.ResumeLayout(false);
            this.pnl_Top.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Refresh)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Open)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Image)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Panel pnl_Top;
        private System.Windows.Forms.Label lbl_LotID;
        private System.Windows.Forms.Label lbl_Result;
        private DevExpress.XtraGrid.GridControl gridControl;
        private DevExpress.XtraGrid.Views.Grid.GridView gridView;
        private System.Windows.Forms.PictureBox pb_Image;
        private System.Windows.Forms.PictureBox pb_Open;
        private System.Windows.Forms.PictureBox pb_Refresh;
        private System.Windows.Forms.CheckBox cb_Position;
    }
}
