namespace HuVision.UI
{
    partial class UcVerify
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
            this.btn_Create = new DevExpress.XtraEditors.SimpleButton();
            this.btn_Path = new DevExpress.XtraEditors.SimpleButton();
            this.tbx_Path = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.gc_BefPst = new DevExpress.XtraGrid.GridControl();
            this.gv_BefPst = new DevExpress.XtraGrid.Views.BandedGrid.BandedGridView();
            this.splitterControl1 = new DevExpress.XtraEditors.SplitterControl();
            this.gc_LstPst = new DevExpress.XtraGrid.GridControl();
            this.gv_LstPst = new DevExpress.XtraGrid.Views.Grid.GridView();
            this.panel2 = new System.Windows.Forms.Panel();
            this.btn_LoadData = new DevExpress.XtraEditors.SimpleButton();
            this.btn_Ef21Create = new DevExpress.XtraEditors.SimpleButton();
            this.btn_Er11Create = new DevExpress.XtraEditors.SimpleButton();
            ((System.ComponentModel.ISupportInitialize)(this.gc_BefPst)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gv_BefPst)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gc_LstPst)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gv_LstPst)).BeginInit();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // btn_Create
            // 
            this.btn_Create.Appearance.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
            this.btn_Create.Appearance.Options.UseFont = true;
            this.btn_Create.Location = new System.Drawing.Point(624, 5);
            this.btn_Create.LookAndFeel.SkinName = "Visual Studio 2013 Dark";
            this.btn_Create.LookAndFeel.UseDefaultLookAndFeel = false;
            this.btn_Create.Name = "btn_Create";
            this.btn_Create.Size = new System.Drawing.Size(131, 34);
            this.btn_Create.TabIndex = 5;
            this.btn_Create.Text = "Creat Standard";
            this.btn_Create.Visible = false;
            this.btn_Create.Click += new System.EventHandler(this.btn_Create_Click);
            // 
            // btn_Path
            // 
            this.btn_Path.Appearance.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
            this.btn_Path.Appearance.Options.UseFont = true;
            this.btn_Path.Location = new System.Drawing.Point(593, 11);
            this.btn_Path.LookAndFeel.SkinName = "Visual Studio 2013 Dark";
            this.btn_Path.LookAndFeel.UseDefaultLookAndFeel = false;
            this.btn_Path.Name = "btn_Path";
            this.btn_Path.Size = new System.Drawing.Size(25, 21);
            this.btn_Path.TabIndex = 4;
            this.btn_Path.Text = "...";
            this.btn_Path.Click += new System.EventHandler(this.btn_Path_Click);
            // 
            // tbx_Path
            // 
            this.tbx_Path.Location = new System.Drawing.Point(44, 11);
            this.tbx_Path.Name = "tbx_Path";
            this.tbx_Path.Size = new System.Drawing.Size(543, 21);
            this.tbx_Path.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold);
            this.label1.Location = new System.Drawing.Point(5, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(44, 14);
            this.label1.TabIndex = 2;
            this.label1.Text = "Path: ";
            // 
            // gc_BefPst
            // 
            this.gc_BefPst.Dock = System.Windows.Forms.DockStyle.Top;
            this.gc_BefPst.Location = new System.Drawing.Point(0, 46);
            this.gc_BefPst.LookAndFeel.SkinName = "Office 2010 Black";
            this.gc_BefPst.LookAndFeel.UseDefaultLookAndFeel = false;
            this.gc_BefPst.MainView = this.gv_BefPst;
            this.gc_BefPst.Name = "gc_BefPst";
            this.gc_BefPst.Size = new System.Drawing.Size(1147, 382);
            this.gc_BefPst.TabIndex = 66;
            this.gc_BefPst.ViewCollection.AddRange(new DevExpress.XtraGrid.Views.Base.BaseView[] {
            this.gv_BefPst});
            // 
            // gv_BefPst
            // 
            this.gv_BefPst.Appearance.BandPanel.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold);
            this.gv_BefPst.Appearance.BandPanel.Options.UseFont = true;
            this.gv_BefPst.Appearance.BandPanel.Options.UseTextOptions = true;
            this.gv_BefPst.Appearance.BandPanel.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gv_BefPst.Appearance.HeaderPanel.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold);
            this.gv_BefPst.Appearance.HeaderPanel.Options.UseFont = true;
            this.gv_BefPst.Appearance.HeaderPanel.Options.UseTextOptions = true;
            this.gv_BefPst.Appearance.HeaderPanel.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gv_BefPst.Appearance.Row.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold);
            this.gv_BefPst.Appearance.Row.Options.UseFont = true;
            this.gv_BefPst.Appearance.Row.Options.UseTextOptions = true;
            this.gv_BefPst.Appearance.Row.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gv_BefPst.FocusRectStyle = DevExpress.XtraGrid.Views.Grid.DrawFocusRectStyle.RowFullFocus;
            this.gv_BefPst.GridControl = this.gc_BefPst;
            this.gv_BefPst.Name = "gv_BefPst";
            this.gv_BefPst.OptionsBehavior.Editable = false;
            this.gv_BefPst.OptionsCustomization.AllowColumnMoving = false;
            this.gv_BefPst.OptionsCustomization.AllowColumnResizing = false;
            this.gv_BefPst.OptionsCustomization.AllowFilter = false;
            this.gv_BefPst.OptionsCustomization.AllowGroup = false;
            this.gv_BefPst.OptionsCustomization.AllowSort = false;
            this.gv_BefPst.OptionsSelection.MultiSelect = true;
            this.gv_BefPst.OptionsView.AllowCellMerge = true;
            this.gv_BefPst.OptionsView.ShowGroupPanel = false;
            this.gv_BefPst.CellMerge += new DevExpress.XtraGrid.Views.Grid.CellMergeEventHandler(this.gv_CellMerge);
            this.gv_BefPst.CustomDrawCell += new DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventHandler(this.gv_BefPst_CustomDrawCell);
            this.gv_BefPst.RowCellStyle += new DevExpress.XtraGrid.Views.Grid.RowCellStyleEventHandler(this.gv_RowCellStyle);
            // 
            // splitterControl1
            // 
            this.splitterControl1.Dock = System.Windows.Forms.DockStyle.Top;
            this.splitterControl1.Location = new System.Drawing.Point(0, 428);
            this.splitterControl1.LookAndFeel.SkinName = "Office 2013 Dark Gray";
            this.splitterControl1.LookAndFeel.UseDefaultLookAndFeel = false;
            this.splitterControl1.Name = "splitterControl1";
            this.splitterControl1.Size = new System.Drawing.Size(1147, 12);
            this.splitterControl1.TabIndex = 67;
            this.splitterControl1.TabStop = false;
            // 
            // gc_LstPst
            // 
            this.gc_LstPst.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gc_LstPst.Location = new System.Drawing.Point(0, 440);
            this.gc_LstPst.LookAndFeel.SkinName = "Office 2010 Black";
            this.gc_LstPst.LookAndFeel.UseDefaultLookAndFeel = false;
            this.gc_LstPst.MainView = this.gv_LstPst;
            this.gc_LstPst.Name = "gc_LstPst";
            this.gc_LstPst.Size = new System.Drawing.Size(1147, 211);
            this.gc_LstPst.TabIndex = 68;
            this.gc_LstPst.ViewCollection.AddRange(new DevExpress.XtraGrid.Views.Base.BaseView[] {
            this.gv_LstPst});
            // 
            // gv_LstPst
            // 
            this.gv_LstPst.Appearance.HeaderPanel.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gv_LstPst.Appearance.HeaderPanel.Options.UseFont = true;
            this.gv_LstPst.Appearance.HeaderPanel.Options.UseTextOptions = true;
            this.gv_LstPst.Appearance.HeaderPanel.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gv_LstPst.Appearance.Row.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.gv_LstPst.Appearance.Row.Options.UseFont = true;
            this.gv_LstPst.Appearance.Row.Options.UseTextOptions = true;
            this.gv_LstPst.Appearance.Row.TextOptions.HAlignment = DevExpress.Utils.HorzAlignment.Center;
            this.gv_LstPst.FocusRectStyle = DevExpress.XtraGrid.Views.Grid.DrawFocusRectStyle.RowFullFocus;
            this.gv_LstPst.GridControl = this.gc_LstPst;
            this.gv_LstPst.Name = "gv_LstPst";
            this.gv_LstPst.OptionsBehavior.Editable = false;
            this.gv_LstPst.OptionsCustomization.AllowColumnMoving = false;
            this.gv_LstPst.OptionsCustomization.AllowColumnResizing = false;
            this.gv_LstPst.OptionsCustomization.AllowFilter = false;
            this.gv_LstPst.OptionsCustomization.AllowGroup = false;
            this.gv_LstPst.OptionsCustomization.AllowSort = false;
            this.gv_LstPst.OptionsSelection.MultiSelect = true;
            this.gv_LstPst.OptionsView.AllowCellMerge = true;
            this.gv_LstPst.OptionsView.ShowGroupPanel = false;
            this.gv_LstPst.CellMerge += new DevExpress.XtraGrid.Views.Grid.CellMergeEventHandler(this.gv_LstPst_CellMerge);
            this.gv_LstPst.CustomDrawCell += new DevExpress.XtraGrid.Views.Base.RowCellCustomDrawEventHandler(this.gv_LstPst_CustomDrawCell);
            this.gv_LstPst.RowCellStyle += new DevExpress.XtraGrid.Views.Grid.RowCellStyleEventHandler(this.gv_RowCellStyle);
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel2.Controls.Add(this.btn_LoadData);
            this.panel2.Controls.Add(this.btn_Ef21Create);
            this.panel2.Controls.Add(this.btn_Er11Create);
            this.panel2.Controls.Add(this.btn_Create);
            this.panel2.Controls.Add(this.tbx_Path);
            this.panel2.Controls.Add(this.btn_Path);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(1147, 46);
            this.panel2.TabIndex = 69;
            // 
            // btn_LoadData
            // 
            this.btn_LoadData.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_LoadData.Appearance.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
            this.btn_LoadData.Appearance.Options.UseFont = true;
            this.btn_LoadData.Location = new System.Drawing.Point(1013, 5);
            this.btn_LoadData.LookAndFeel.SkinName = "Visual Studio 2013 Dark";
            this.btn_LoadData.LookAndFeel.UseDefaultLookAndFeel = false;
            this.btn_LoadData.Name = "btn_LoadData";
            this.btn_LoadData.Size = new System.Drawing.Size(120, 34);
            this.btn_LoadData.TabIndex = 9;
            this.btn_LoadData.Text = "Load LastData";
            this.btn_LoadData.Click += new System.EventHandler(this.btn_LoadData_Click);
            // 
            // btn_Ef21Create
            // 
            this.btn_Ef21Create.Appearance.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
            this.btn_Ef21Create.Appearance.Options.UseFont = true;
            this.btn_Ef21Create.Location = new System.Drawing.Point(731, 5);
            this.btn_Ef21Create.LookAndFeel.SkinName = "Visual Studio 2013 Dark";
            this.btn_Ef21Create.LookAndFeel.UseDefaultLookAndFeel = false;
            this.btn_Ef21Create.Name = "btn_Ef21Create";
            this.btn_Ef21Create.Size = new System.Drawing.Size(101, 34);
            this.btn_Ef21Create.TabIndex = 8;
            this.btn_Ef21Create.Text = "EF21 Create";
            this.btn_Ef21Create.Visible = false;
            this.btn_Ef21Create.Click += new System.EventHandler(this.btn_Ef21Create_Click);
            // 
            // btn_Er11Create
            // 
            this.btn_Er11Create.Appearance.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Bold);
            this.btn_Er11Create.Appearance.Options.UseFont = true;
            this.btn_Er11Create.Location = new System.Drawing.Point(624, 18);
            this.btn_Er11Create.LookAndFeel.SkinName = "Visual Studio 2013 Dark";
            this.btn_Er11Create.LookAndFeel.UseDefaultLookAndFeel = false;
            this.btn_Er11Create.Name = "btn_Er11Create";
            this.btn_Er11Create.Size = new System.Drawing.Size(101, 34);
            this.btn_Er11Create.TabIndex = 7;
            this.btn_Er11Create.Text = "ER11 Create";
            this.btn_Er11Create.Visible = false;
            this.btn_Er11Create.Click += new System.EventHandler(this.btn_Er11Create_Click);
            // 
            // UcVerify
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.gc_LstPst);
            this.Controls.Add(this.splitterControl1);
            this.Controls.Add(this.gc_BefPst);
            this.Controls.Add(this.panel2);
            this.Name = "UcVerify";
            this.Size = new System.Drawing.Size(1147, 651);
            this.Load += new System.EventHandler(this.UcVerify_Load);
            ((System.ComponentModel.ISupportInitialize)(this.gc_BefPst)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gv_BefPst)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gc_LstPst)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gv_LstPst)).EndInit();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private DevExpress.XtraGrid.GridControl gc_BefPst;
        private DevExpress.XtraEditors.SplitterControl splitterControl1;
        private DevExpress.XtraGrid.GridControl gc_LstPst;
        private DevExpress.XtraGrid.Views.Grid.GridView gv_LstPst;
        private DevExpress.XtraGrid.Views.BandedGrid.BandedGridView gv_BefPst;
        private DevExpress.XtraEditors.SimpleButton btn_Create;
        private DevExpress.XtraEditors.SimpleButton btn_Path;
        private System.Windows.Forms.TextBox tbx_Path;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel2;
        private DevExpress.XtraEditors.SimpleButton btn_Ef21Create;
        private DevExpress.XtraEditors.SimpleButton btn_Er11Create;
        private DevExpress.XtraEditors.SimpleButton btn_LoadData;
    }
}
