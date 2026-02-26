namespace HuVision.UI
{
    partial class UcRework
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
            this.groupBox = new System.Windows.Forms.GroupBox();
            this.cb_1 = new System.Windows.Forms.CheckBox();
            this.cb_2 = new System.Windows.Forms.CheckBox();
            this.cb_4 = new System.Windows.Forms.CheckBox();
            this.cb_3 = new System.Windows.Forms.CheckBox();
            this.groupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox
            // 
            this.groupBox.Controls.Add(this.cb_4);
            this.groupBox.Controls.Add(this.cb_3);
            this.groupBox.Controls.Add(this.cb_2);
            this.groupBox.Controls.Add(this.cb_1);
            this.groupBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.groupBox.Location = new System.Drawing.Point(0, 0);
            this.groupBox.Name = "groupBox";
            this.groupBox.Size = new System.Drawing.Size(58, 117);
            this.groupBox.TabIndex = 0;
            this.groupBox.TabStop = false;
            this.groupBox.Text = "0";
            // 
            // cb_1
            // 
            this.cb_1.AutoSize = true;
            this.cb_1.Location = new System.Drawing.Point(15, 20);
            this.cb_1.Name = "cb_1";
            this.cb_1.Size = new System.Drawing.Size(30, 16);
            this.cb_1.TabIndex = 0;
            this.cb_1.Text = "1";
            this.cb_1.UseVisualStyleBackColor = true;
            // 
            // cb_2
            // 
            this.cb_2.AutoSize = true;
            this.cb_2.Location = new System.Drawing.Point(15, 42);
            this.cb_2.Name = "cb_2";
            this.cb_2.Size = new System.Drawing.Size(30, 16);
            this.cb_2.TabIndex = 1;
            this.cb_2.Text = "2";
            this.cb_2.UseVisualStyleBackColor = true;
            // 
            // cb_4
            // 
            this.cb_4.AutoSize = true;
            this.cb_4.Location = new System.Drawing.Point(15, 86);
            this.cb_4.Name = "cb_4";
            this.cb_4.Size = new System.Drawing.Size(30, 16);
            this.cb_4.TabIndex = 3;
            this.cb_4.Text = "4";
            this.cb_4.UseVisualStyleBackColor = true;
            // 
            // cb_3
            // 
            this.cb_3.AutoSize = true;
            this.cb_3.Location = new System.Drawing.Point(15, 64);
            this.cb_3.Name = "cb_3";
            this.cb_3.Size = new System.Drawing.Size(30, 16);
            this.cb_3.TabIndex = 2;
            this.cb_3.Text = "3";
            this.cb_3.UseVisualStyleBackColor = true;
            // 
            // UcRework
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.groupBox);
            this.Name = "UcRework";
            this.Size = new System.Drawing.Size(58, 117);
            this.groupBox.ResumeLayout(false);
            this.groupBox.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox;
        private System.Windows.Forms.CheckBox cb_4;
        private System.Windows.Forms.CheckBox cb_3;
        private System.Windows.Forms.CheckBox cb_2;
        private System.Windows.Forms.CheckBox cb_1;
    }
}
