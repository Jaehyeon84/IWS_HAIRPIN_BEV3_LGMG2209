namespace MIS
{
    partial class MainForm
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

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.notifyIcon1 = new System.Windows.Forms.NotifyIcon(this.components);
            this.button_PLC_InputA = new System.Windows.Forms.Button();
            this.button_PLC_outputA = new System.Windows.Forms.Button();
            this.button_PLC_ErrorA = new System.Windows.Forms.Button();
            this.btn_Clear_GMES = new System.Windows.Forms.Button();
            this.btn_Clear_GMESA = new System.Windows.Forms.Button();
            this.buttonClearPLC = new System.Windows.Forms.Button();
            this.ucMISStatus = new MIS.ucStatus();
            this.SuspendLayout();
            // 
            // notifyIcon1
            // 
            this.notifyIcon1.Icon = ((System.Drawing.Icon)(resources.GetObject("notifyIcon1.Icon")));
            this.notifyIcon1.Text = "GMES Interface System(#95)";
            this.notifyIcon1.Visible = true;
            this.notifyIcon1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.notifyIcon1_MouseDoubleClick);
            // 
            // button_PLC_InputA
            // 
            this.button_PLC_InputA.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.button_PLC_InputA.Location = new System.Drawing.Point(374, 0);
            this.button_PLC_InputA.Name = "button_PLC_InputA";
            this.button_PLC_InputA.Size = new System.Drawing.Size(126, 29);
            this.button_PLC_InputA.TabIndex = 8;
            this.button_PLC_InputA.Text = "InputA Response";
            this.button_PLC_InputA.UseVisualStyleBackColor = false;
            this.button_PLC_InputA.Click += new System.EventHandler(this.button_PLC_InputA_Click);
            // 
            // button_PLC_outputA
            // 
            this.button_PLC_outputA.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.button_PLC_outputA.Location = new System.Drawing.Point(374, 35);
            this.button_PLC_outputA.Name = "button_PLC_outputA";
            this.button_PLC_outputA.Size = new System.Drawing.Size(126, 29);
            this.button_PLC_outputA.TabIndex = 10;
            this.button_PLC_outputA.Text = "OutputA Response";
            this.button_PLC_outputA.UseVisualStyleBackColor = false;
            this.button_PLC_outputA.Click += new System.EventHandler(this.button_PLC_outputA_Click);
            // 
            // button_PLC_ErrorA
            // 
            this.button_PLC_ErrorA.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.button_PLC_ErrorA.Location = new System.Drawing.Point(374, 70);
            this.button_PLC_ErrorA.Name = "button_PLC_ErrorA";
            this.button_PLC_ErrorA.Size = new System.Drawing.Size(126, 29);
            this.button_PLC_ErrorA.TabIndex = 11;
            this.button_PLC_ErrorA.Text = "Input Error A";
            this.button_PLC_ErrorA.UseVisualStyleBackColor = false;
            this.button_PLC_ErrorA.Click += new System.EventHandler(this.button_PLC_ErrorA_Click);
            // 
            // btn_Clear_GMES
            // 
            this.btn_Clear_GMES.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Clear_GMES.Location = new System.Drawing.Point(107, 72);
            this.btn_Clear_GMES.Name = "btn_Clear_GMES";
            this.btn_Clear_GMES.Size = new System.Drawing.Size(126, 29);
            this.btn_Clear_GMES.TabIndex = 14;
            this.btn_Clear_GMES.Text = "Clear MES Flag";
            this.btn_Clear_GMES.UseVisualStyleBackColor = false;
            this.btn_Clear_GMES.Click += new System.EventHandler(this.btn_Clear_GMES_Click);
            // 
            // btn_Clear_GMESA
            // 
            this.btn_Clear_GMESA.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.btn_Clear_GMESA.Location = new System.Drawing.Point(240, 72);
            this.btn_Clear_GMESA.Name = "btn_Clear_GMESA";
            this.btn_Clear_GMESA.Size = new System.Drawing.Size(126, 29);
            this.btn_Clear_GMESA.TabIndex = 16;
            this.btn_Clear_GMESA.Text = "Clear MES AJIg";
            this.btn_Clear_GMESA.UseVisualStyleBackColor = false;
            this.btn_Clear_GMESA.Click += new System.EventHandler(this.btn_Clear_GMESA_Click);
            // 
            // buttonClearPLC
            // 
            this.buttonClearPLC.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.buttonClearPLC.Location = new System.Drawing.Point(240, 37);
            this.buttonClearPLC.Name = "buttonClearPLC";
            this.buttonClearPLC.Size = new System.Drawing.Size(126, 30);
            this.buttonClearPLC.TabIndex = 17;
            this.buttonClearPLC.Text = "Reset GMES";
            this.buttonClearPLC.UseVisualStyleBackColor = false;
            this.buttonClearPLC.Click += new System.EventHandler(this.buttonClearPLC_Click);
            // 
            // ucMISStatus
            // 
            this.ucMISStatus.BackColor = System.Drawing.Color.Black;
            this.ucMISStatus.Dock = System.Windows.Forms.DockStyle.Fill;
            this.ucMISStatus.Location = new System.Drawing.Point(0, 0);
            this.ucMISStatus.Name = "ucMISStatus";
            this.ucMISStatus.Size = new System.Drawing.Size(515, 585);
            this.ucMISStatus.TabIndex = 7;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(515, 585);
            this.Controls.Add(this.buttonClearPLC);
            this.Controls.Add(this.btn_Clear_GMESA);
            this.Controls.Add(this.btn_Clear_GMES);
            this.Controls.Add(this.button_PLC_ErrorA);
            this.Controls.Add(this.button_PLC_outputA);
            this.Controls.Add(this.button_PLC_InputA);
            this.Controls.Add(this.ucMISStatus);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.Text = "GMES Interface System(#95)";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainFormClosing);
            this.Resize += new System.EventHandler(this.MainForm_Resize);
            this.ResumeLayout(false);

        }

        #endregion

        private ucStatus ucMISStatus;
        private System.Windows.Forms.NotifyIcon notifyIcon1;
        private System.Windows.Forms.Button button_PLC_InputA;
        private System.Windows.Forms.Button button_PLC_outputA;
        private System.Windows.Forms.Button button_PLC_ErrorA;
        private System.Windows.Forms.Button btn_Clear_GMES;
        private System.Windows.Forms.Button btn_Clear_GMESA;
        private System.Windows.Forms.Button buttonClearPLC;
    }
}

