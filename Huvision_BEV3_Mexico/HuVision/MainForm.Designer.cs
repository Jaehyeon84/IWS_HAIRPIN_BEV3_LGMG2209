namespace HuVision
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.pnl_Top = new System.Windows.Forms.Panel();
            this.pb_Config = new DevExpress.XtraEditors.PictureEdit();
            this.lbl_Name = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.pnl_State = new System.Windows.Forms.Panel();
            this.WriteTest = new System.Windows.Forms.Button();
            this.btn_Load = new System.Windows.Forms.Button();
            this.tbx_Model = new System.Windows.Forms.TextBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.lbl_Result = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.pb_OutLink = new System.Windows.Forms.PictureBox();
            this.label19 = new System.Windows.Forms.Label();
            this.pb_InspRes = new System.Windows.Forms.PictureBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.lbl_Model = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.lbl_LotID = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.lbl_Rcp = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.pb_InLink = new System.Windows.Forms.PictureBox();
            this.label8 = new System.Windows.Forms.Label();
            this.pb_InspReq = new System.Windows.Forms.PictureBox();
            this.cb_Manual = new System.Windows.Forms.CheckBox();
            this.btn_ManualSave = new System.Windows.Forms.Button();
            this.tbx_ManualPath = new System.Windows.Forms.TextBox();
            this.lbl_Path = new System.Windows.Forms.Label();
            this.pnl_Main = new System.Windows.Forms.Panel();
            this.pnl_Top.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Config.Properties)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.pnl_State.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_OutLink)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_InspRes)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_InLink)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_InspReq)).BeginInit();
            this.SuspendLayout();
            // 
            // pnl_Top
            // 
            this.pnl_Top.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(41)))), ((int)(((byte)(151)))), ((int)(((byte)(239)))));
            this.pnl_Top.Controls.Add(this.pb_Config);
            this.pnl_Top.Controls.Add(this.lbl_Name);
            this.pnl_Top.Controls.Add(this.label1);
            this.pnl_Top.Controls.Add(this.pictureBox1);
            this.pnl_Top.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnl_Top.Location = new System.Drawing.Point(0, 0);
            this.pnl_Top.Name = "pnl_Top";
            this.pnl_Top.Size = new System.Drawing.Size(1263, 59);
            this.pnl_Top.TabIndex = 0;
            // 
            // pb_Config
            // 
            this.pb_Config.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pb_Config.Cursor = System.Windows.Forms.Cursors.Hand;
            this.pb_Config.EditValue = ((object)(resources.GetObject("pb_Config.EditValue")));
            this.pb_Config.Location = new System.Drawing.Point(1211, 9);
            this.pb_Config.Name = "pb_Config";
            // 
            // 
            // 
            this.pb_Config.Properties.Appearance.BackColor = System.Drawing.Color.Transparent;
            this.pb_Config.Properties.Appearance.Options.UseBackColor = true;
            this.pb_Config.Properties.BorderStyle = DevExpress.XtraEditors.Controls.BorderStyles.NoBorder;
            this.pb_Config.Properties.ShowCameraMenuItem = DevExpress.XtraEditors.Controls.CameraMenuItemVisibility.Auto;
            this.pb_Config.Size = new System.Drawing.Size(40, 40);
            this.pb_Config.TabIndex = 6;
            this.pb_Config.Click += new System.EventHandler(this.pb_Config_Click);
            // 
            // lbl_Name
            // 
            this.lbl_Name.AutoSize = true;
            this.lbl_Name.Font = new System.Drawing.Font("Consolas", 15F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbl_Name.ForeColor = System.Drawing.Color.White;
            this.lbl_Name.Location = new System.Drawing.Point(440, 29);
            this.lbl_Name.Name = "lbl_Name";
            this.lbl_Name.Size = new System.Drawing.Size(21, 23);
            this.lbl_Name.TabIndex = 5;
            this.lbl_Name.Text = "-";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Consolas", 15F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(212, 29);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(230, 23);
            this.label1.TabIndex = 3;
            this.label1.Text = "Hubis Welding Vision";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::HuVision.Properties.Resources.LGEW;
            this.pictureBox1.Location = new System.Drawing.Point(5, 4);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(201, 50);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 1;
            this.pictureBox1.TabStop = false;
            // 
            // pnl_State
            // 
            this.pnl_State.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(62)))), ((int)(((byte)(67)))), ((int)(((byte)(79)))));
            this.pnl_State.Controls.Add(this.WriteTest);
            this.pnl_State.Controls.Add(this.btn_Load);
            this.pnl_State.Controls.Add(this.tbx_Model);
            this.pnl_State.Controls.Add(this.groupBox2);
            this.pnl_State.Controls.Add(this.groupBox1);
            this.pnl_State.Controls.Add(this.cb_Manual);
            this.pnl_State.Controls.Add(this.btn_ManualSave);
            this.pnl_State.Controls.Add(this.tbx_ManualPath);
            this.pnl_State.Controls.Add(this.lbl_Path);
            this.pnl_State.Dock = System.Windows.Forms.DockStyle.Right;
            this.pnl_State.Location = new System.Drawing.Point(1069, 59);
            this.pnl_State.Name = "pnl_State";
            this.pnl_State.Padding = new System.Windows.Forms.Padding(0, 0, 0, 10);
            this.pnl_State.Size = new System.Drawing.Size(194, 818);
            this.pnl_State.TabIndex = 1;
            // 
            // WriteTest
            // 
            this.WriteTest.Location = new System.Drawing.Point(9, 248);
            this.WriteTest.Name = "WriteTest";
            this.WriteTest.Size = new System.Drawing.Size(181, 23);
            this.WriteTest.TabIndex = 40;
            this.WriteTest.Text = "Make Previous Data";
            this.WriteTest.UseVisualStyleBackColor = true;
            this.WriteTest.Click += new System.EventHandler(this.WriteTest_Click);
            // 
            // btn_Load
            // 
            this.btn_Load.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn_Load.Location = new System.Drawing.Point(51, 753);
            this.btn_Load.Name = "btn_Load";
            this.btn_Load.Size = new System.Drawing.Size(118, 23);
            this.btn_Load.TabIndex = 39;
            this.btn_Load.Text = "Load";
            this.btn_Load.UseVisualStyleBackColor = true;
            this.btn_Load.Visible = false;
            this.btn_Load.Click += new System.EventHandler(this.btn_Load_Click);
            // 
            // tbx_Model
            // 
            this.tbx_Model.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.tbx_Model.Location = new System.Drawing.Point(126, 699);
            this.tbx_Model.Name = "tbx_Model";
            this.tbx_Model.Size = new System.Drawing.Size(42, 21);
            this.tbx_Model.TabIndex = 38;
            this.tbx_Model.Text = "1";
            this.tbx_Model.Visible = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.lbl_Result);
            this.groupBox2.Controls.Add(this.label23);
            this.groupBox2.Controls.Add(this.label17);
            this.groupBox2.Controls.Add(this.pb_OutLink);
            this.groupBox2.Controls.Add(this.label19);
            this.groupBox2.Controls.Add(this.pb_InspRes);
            this.groupBox2.ForeColor = System.Drawing.Color.White;
            this.groupBox2.Location = new System.Drawing.Point(9, 156);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(182, 87);
            this.groupBox2.TabIndex = 34;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Out";
            // 
            // lbl_Result
            // 
            this.lbl_Result.AutoSize = true;
            this.lbl_Result.Font = new System.Drawing.Font("Tahoma", 10F);
            this.lbl_Result.ForeColor = System.Drawing.Color.White;
            this.lbl_Result.Location = new System.Drawing.Point(140, 62);
            this.lbl_Result.Name = "lbl_Result";
            this.lbl_Result.Size = new System.Drawing.Size(13, 17);
            this.lbl_Result.TabIndex = 38;
            this.lbl_Result.Text = "-";
            this.lbl_Result.Visible = false;
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label23.ForeColor = System.Drawing.Color.White;
            this.label23.Location = new System.Drawing.Point(9, 60);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(45, 17);
            this.label23.TabIndex = 37;
            this.label23.Text = "Result";
            this.label23.Visible = false;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label17.ForeColor = System.Drawing.Color.White;
            this.label17.Location = new System.Drawing.Point(26, 17);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(32, 17);
            this.label17.TabIndex = 27;
            this.label17.Text = "Link";
            // 
            // pb_OutLink
            // 
            this.pb_OutLink.Image = global::HuVision.Properties.Resources.Toggle_off;
            this.pb_OutLink.Location = new System.Drawing.Point(9, 20);
            this.pb_OutLink.Name = "pb_OutLink";
            this.pb_OutLink.Size = new System.Drawing.Size(13, 13);
            this.pb_OutLink.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pb_OutLink.TabIndex = 26;
            this.pb_OutLink.TabStop = false;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label19.ForeColor = System.Drawing.Color.White;
            this.label19.Location = new System.Drawing.Point(26, 36);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(37, 17);
            this.label19.TabIndex = 23;
            this.label19.Text = "INSP";
            // 
            // pb_InspRes
            // 
            this.pb_InspRes.Image = global::HuVision.Properties.Resources.Toggle_off;
            this.pb_InspRes.Location = new System.Drawing.Point(9, 39);
            this.pb_InspRes.Name = "pb_InspRes";
            this.pb_InspRes.Size = new System.Drawing.Size(13, 13);
            this.pb_InspRes.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pb_InspRes.TabIndex = 6;
            this.pb_InspRes.TabStop = false;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.lbl_Model);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.lbl_LotID);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.lbl_Rcp);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.pb_InLink);
            this.groupBox1.Controls.Add(this.label8);
            this.groupBox1.Controls.Add(this.pb_InspReq);
            this.groupBox1.ForeColor = System.Drawing.Color.White;
            this.groupBox1.Location = new System.Drawing.Point(9, 7);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(182, 143);
            this.groupBox1.TabIndex = 22;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "IN";
            // 
            // lbl_Model
            // 
            this.lbl_Model.AutoSize = true;
            this.lbl_Model.Font = new System.Drawing.Font("Tahoma", 10F);
            this.lbl_Model.ForeColor = System.Drawing.Color.White;
            this.lbl_Model.Location = new System.Drawing.Point(58, 61);
            this.lbl_Model.Name = "lbl_Model";
            this.lbl_Model.Size = new System.Drawing.Size(13, 17);
            this.lbl_Model.TabIndex = 45;
            this.lbl_Model.Text = "-";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label4.ForeColor = System.Drawing.Color.White;
            this.label4.Location = new System.Drawing.Point(9, 61);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(43, 17);
            this.label4.TabIndex = 44;
            this.label4.Text = "Model";
            // 
            // lbl_LotID
            // 
            this.lbl_LotID.AutoSize = true;
            this.lbl_LotID.Font = new System.Drawing.Font("Tahoma", 10F);
            this.lbl_LotID.ForeColor = System.Drawing.Color.White;
            this.lbl_LotID.Location = new System.Drawing.Point(57, 112);
            this.lbl_LotID.Name = "lbl_LotID";
            this.lbl_LotID.Size = new System.Drawing.Size(13, 17);
            this.lbl_LotID.TabIndex = 43;
            this.lbl_LotID.Text = "-";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label5.ForeColor = System.Drawing.Color.White;
            this.label5.Location = new System.Drawing.Point(9, 113);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(42, 17);
            this.label5.TabIndex = 42;
            this.label5.Text = "LotID";
            // 
            // lbl_Rcp
            // 
            this.lbl_Rcp.AutoSize = true;
            this.lbl_Rcp.Font = new System.Drawing.Font("Tahoma", 10F);
            this.lbl_Rcp.ForeColor = System.Drawing.Color.White;
            this.lbl_Rcp.Location = new System.Drawing.Point(58, 85);
            this.lbl_Rcp.Name = "lbl_Rcp";
            this.lbl_Rcp.Size = new System.Drawing.Size(13, 17);
            this.lbl_Rcp.TabIndex = 39;
            this.lbl_Rcp.Text = "-";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(9, 86);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(32, 17);
            this.label2.TabIndex = 39;
            this.label2.Text = "Rcp";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label10.ForeColor = System.Drawing.Color.White;
            this.label10.Location = new System.Drawing.Point(26, 17);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(32, 17);
            this.label10.TabIndex = 27;
            this.label10.Text = "Link";
            // 
            // pb_InLink
            // 
            this.pb_InLink.Image = global::HuVision.Properties.Resources.Toggle_off;
            this.pb_InLink.Location = new System.Drawing.Point(9, 20);
            this.pb_InLink.Name = "pb_InLink";
            this.pb_InLink.Size = new System.Drawing.Size(13, 13);
            this.pb_InLink.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pb_InLink.TabIndex = 26;
            this.pb_InLink.TabStop = false;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Tahoma", 10F);
            this.label8.ForeColor = System.Drawing.Color.White;
            this.label8.Location = new System.Drawing.Point(26, 37);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(37, 17);
            this.label8.TabIndex = 23;
            this.label8.Text = "INSP";
            // 
            // pb_InspReq
            // 
            this.pb_InspReq.Image = global::HuVision.Properties.Resources.Toggle_off;
            this.pb_InspReq.Location = new System.Drawing.Point(9, 40);
            this.pb_InspReq.Name = "pb_InspReq";
            this.pb_InspReq.Size = new System.Drawing.Size(13, 13);
            this.pb_InspReq.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pb_InspReq.TabIndex = 6;
            this.pb_InspReq.TabStop = false;
            this.pb_InspReq.Click += new System.EventHandler(this.pb_Click);
            // 
            // cb_Manual
            // 
            this.cb_Manual.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.cb_Manual.AutoSize = true;
            this.cb_Manual.ForeColor = System.Drawing.Color.White;
            this.cb_Manual.Location = new System.Drawing.Point(13, 704);
            this.cb_Manual.Name = "cb_Manual";
            this.cb_Manual.Size = new System.Drawing.Size(66, 16);
            this.cb_Manual.TabIndex = 21;
            this.cb_Manual.Text = "Manual";
            this.cb_Manual.UseVisualStyleBackColor = true;
            this.cb_Manual.Visible = false;
            this.cb_Manual.CheckedChanged += new System.EventHandler(this.cb_Manual_CheckedChanged);
            // 
            // btn_ManualSave
            // 
            this.btn_ManualSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn_ManualSave.Location = new System.Drawing.Point(51, 781);
            this.btn_ManualSave.Name = "btn_ManualSave";
            this.btn_ManualSave.Size = new System.Drawing.Size(118, 23);
            this.btn_ManualSave.TabIndex = 20;
            this.btn_ManualSave.Text = "Save";
            this.btn_ManualSave.UseVisualStyleBackColor = true;
            this.btn_ManualSave.Visible = false;
            this.btn_ManualSave.Click += new System.EventHandler(this.btn_ManualSave_Click);
            // 
            // tbx_ManualPath
            // 
            this.tbx_ManualPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.tbx_ManualPath.Location = new System.Drawing.Point(51, 726);
            this.tbx_ManualPath.Name = "tbx_ManualPath";
            this.tbx_ManualPath.Size = new System.Drawing.Size(118, 21);
            this.tbx_ManualPath.TabIndex = 18;
            this.tbx_ManualPath.Visible = false;
            // 
            // lbl_Path
            // 
            this.lbl_Path.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lbl_Path.AutoSize = true;
            this.lbl_Path.Font = new System.Drawing.Font("Tahoma", 9F);
            this.lbl_Path.ForeColor = System.Drawing.Color.White;
            this.lbl_Path.Location = new System.Drawing.Point(10, 730);
            this.lbl_Path.Name = "lbl_Path";
            this.lbl_Path.Size = new System.Drawing.Size(32, 14);
            this.lbl_Path.TabIndex = 16;
            this.lbl_Path.Text = "Path";
            this.lbl_Path.Visible = false;
            // 
            // pnl_Main
            // 
            this.pnl_Main.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnl_Main.Location = new System.Drawing.Point(0, 59);
            this.pnl_Main.Name = "pnl_Main";
            this.pnl_Main.Padding = new System.Windows.Forms.Padding(5, 5, 5, 5);
            this.pnl_Main.Size = new System.Drawing.Size(1069, 818);
            this.pnl_Main.TabIndex = 4;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(1263, 877);
            this.Controls.Add(this.pnl_Main);
            this.Controls.Add(this.pnl_State);
            this.Controls.Add(this.pnl_Top);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.Text = "HuVision";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.pnl_Top.ResumeLayout(false);
            this.pnl_Top.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_Config.Properties)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.pnl_State.ResumeLayout(false);
            this.pnl_State.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_OutLink)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_InspRes)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_InLink)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_InspReq)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel pnl_Top;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel pnl_State;
        private System.Windows.Forms.Label lbl_Name;
        private System.Windows.Forms.CheckBox cb_Manual;
        private System.Windows.Forms.Button btn_ManualSave;
        private System.Windows.Forms.TextBox tbx_ManualPath;
        private System.Windows.Forms.Label lbl_Path;
        private System.Windows.Forms.Panel pnl_Main;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lbl_Result;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.PictureBox pb_OutLink;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.PictureBox pb_InspRes;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.PictureBox pb_InLink;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.PictureBox pb_InspReq;
        private System.Windows.Forms.Label lbl_Rcp;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label lbl_LotID;
        private System.Windows.Forms.Label lbl_Model;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbx_Model;
        private System.Windows.Forms.Button btn_Load;
        private DevExpress.XtraEditors.PictureEdit pb_Config;
        private System.Windows.Forms.Button WriteTest;
    }
}

