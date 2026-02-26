namespace HuVision.UI
{
    partial class FrmVerifyConfig
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
            this.label2 = new System.Windows.Forms.Label();
            this.tbx_Threshold = new System.Windows.Forms.TextBox();
            this.btn_Save = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cb_UseR = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tbx_JumpMax = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.cb_UseJump = new System.Windows.Forms.CheckBox();
            this.tbx_JumpMin = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Tahoma", 12F);
            this.label2.ForeColor = System.Drawing.Color.Black;
            this.label2.Location = new System.Drawing.Point(7, 54);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(80, 19);
            this.label2.TabIndex = 5;
            this.label2.Text = "Threshold";
            // 
            // tbx_Threshold
            // 
            this.tbx_Threshold.Location = new System.Drawing.Point(96, 54);
            this.tbx_Threshold.Name = "tbx_Threshold";
            this.tbx_Threshold.Size = new System.Drawing.Size(93, 21);
            this.tbx_Threshold.TabIndex = 6;
            this.tbx_Threshold.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbx_KeyPress);
            // 
            // btn_Save
            // 
            this.btn_Save.Font = new System.Drawing.Font("굴림", 12F, System.Drawing.FontStyle.Bold);
            this.btn_Save.Location = new System.Drawing.Point(12, 229);
            this.btn_Save.Name = "btn_Save";
            this.btn_Save.Size = new System.Drawing.Size(195, 34);
            this.btn_Save.TabIndex = 25;
            this.btn_Save.Text = "Save";
            this.btn_Save.UseVisualStyleBackColor = true;
            this.btn_Save.Click += new System.EventHandler(this.btn_Save_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.cb_UseR);
            this.groupBox1.Controls.Add(this.tbx_Threshold);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(195, 87);
            this.groupBox1.TabIndex = 28;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "좌표 반경 검사";
            // 
            // cb_UseR
            // 
            this.cb_UseR.AutoSize = true;
            this.cb_UseR.Font = new System.Drawing.Font("Tahoma", 12F);
            this.cb_UseR.Location = new System.Drawing.Point(11, 24);
            this.cb_UseR.Name = "cb_UseR";
            this.cb_UseR.Size = new System.Drawing.Size(54, 23);
            this.cb_UseR.TabIndex = 0;
            this.cb_UseR.Text = "Use";
            this.cb_UseR.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.tbx_JumpMax);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.cb_UseJump);
            this.groupBox2.Controls.Add(this.tbx_JumpMin);
            this.groupBox2.Location = new System.Drawing.Point(12, 105);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(195, 118);
            this.groupBox2.TabIndex = 29;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Jump 구간 검사";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Tahoma", 12F);
            this.label3.ForeColor = System.Drawing.Color.Black;
            this.label3.Location = new System.Drawing.Point(12, 83);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(37, 19);
            this.label3.TabIndex = 7;
            this.label3.Text = "Max";
            // 
            // tbx_JumpMax
            // 
            this.tbx_JumpMax.Location = new System.Drawing.Point(96, 83);
            this.tbx_JumpMax.Name = "tbx_JumpMax";
            this.tbx_JumpMax.Size = new System.Drawing.Size(93, 21);
            this.tbx_JumpMax.TabIndex = 8;
            this.tbx_JumpMax.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbx_KeyPress);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Tahoma", 12F);
            this.label1.ForeColor = System.Drawing.Color.Black;
            this.label1.Location = new System.Drawing.Point(12, 56);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(34, 19);
            this.label1.TabIndex = 5;
            this.label1.Text = "Min";
            // 
            // cb_UseJump
            // 
            this.cb_UseJump.AutoSize = true;
            this.cb_UseJump.Font = new System.Drawing.Font("Tahoma", 12F);
            this.cb_UseJump.Location = new System.Drawing.Point(11, 24);
            this.cb_UseJump.Name = "cb_UseJump";
            this.cb_UseJump.Size = new System.Drawing.Size(54, 23);
            this.cb_UseJump.TabIndex = 0;
            this.cb_UseJump.Text = "Use";
            this.cb_UseJump.UseVisualStyleBackColor = true;
            // 
            // tbx_JumpMin
            // 
            this.tbx_JumpMin.Location = new System.Drawing.Point(96, 56);
            this.tbx_JumpMin.Name = "tbx_JumpMin";
            this.tbx_JumpMin.Size = new System.Drawing.Size(93, 21);
            this.tbx_JumpMin.TabIndex = 6;
            this.tbx_JumpMin.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.tbx_KeyPress);
            // 
            // FrmVerifyConfig
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(224, 282);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.btn_Save);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "FrmVerifyConfig";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Config";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbx_Threshold;
        private System.Windows.Forms.Button btn_Save;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox cb_UseR;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox cb_UseJump;
        private System.Windows.Forms.TextBox tbx_JumpMin;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbx_JumpMax;

    }
}