namespace HuVision.UI
{
    partial class FrmInspView
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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.tbx_AftData = new System.Windows.Forms.TextBox();
            this.lbl_AftLotID = new System.Windows.Forms.Label();
            this.lbl_BefLotID = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tbx_BefData = new System.Windows.Forms.TextBox();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 2;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 1F));
            this.tableLayoutPanel1.Controls.Add(this.tbx_AftData, 1, 2);
            this.tableLayoutPanel1.Controls.Add(this.lbl_AftLotID, 1, 1);
            this.tableLayoutPanel1.Controls.Add(this.lbl_BefLotID, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.label2, 1, 0);
            this.tableLayoutPanel1.Controls.Add(this.label1, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.tbx_BefData, 0, 2);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 3;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(510, 667);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // tbx_AftData
            // 
            this.tbx_AftData.BackColor = System.Drawing.Color.White;
            this.tbx_AftData.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbx_AftData.Font = new System.Drawing.Font("Consolas", 15F, System.Drawing.FontStyle.Bold);
            this.tbx_AftData.ForeColor = System.Drawing.Color.Black;
            this.tbx_AftData.Location = new System.Drawing.Point(512, 83);
            this.tbx_AftData.Multiline = true;
            this.tbx_AftData.Name = "tbx_AftData";
            this.tbx_AftData.ReadOnly = true;
            this.tbx_AftData.Size = new System.Drawing.Size(1, 581);
            this.tbx_AftData.TabIndex = 9;
            // 
            // lbl_AftLotID
            // 
            this.lbl_AftLotID.AutoSize = true;
            this.lbl_AftLotID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(254)))), ((int)(((byte)(137)))), ((int)(((byte)(106)))));
            this.lbl_AftLotID.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lbl_AftLotID.Font = new System.Drawing.Font("Consolas", 15F, System.Drawing.FontStyle.Bold);
            this.lbl_AftLotID.ForeColor = System.Drawing.Color.White;
            this.lbl_AftLotID.Location = new System.Drawing.Point(512, 40);
            this.lbl_AftLotID.Name = "lbl_AftLotID";
            this.lbl_AftLotID.Size = new System.Drawing.Size(1, 40);
            this.lbl_AftLotID.TabIndex = 7;
            this.lbl_AftLotID.Text = "-";
            this.lbl_AftLotID.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // lbl_BefLotID
            // 
            this.lbl_BefLotID.AutoSize = true;
            this.lbl_BefLotID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(254)))), ((int)(((byte)(137)))), ((int)(((byte)(106)))));
            this.lbl_BefLotID.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lbl_BefLotID.Font = new System.Drawing.Font("Consolas", 15F, System.Drawing.FontStyle.Bold);
            this.lbl_BefLotID.ForeColor = System.Drawing.Color.White;
            this.lbl_BefLotID.Location = new System.Drawing.Point(3, 40);
            this.lbl_BefLotID.Name = "lbl_BefLotID";
            this.lbl_BefLotID.Size = new System.Drawing.Size(503, 40);
            this.lbl_BefLotID.TabIndex = 6;
            this.lbl_BefLotID.Text = "-";
            this.lbl_BefLotID.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(80)))), ((int)(((byte)(80)))));
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Font = new System.Drawing.Font("Consolas", 20F, System.Drawing.FontStyle.Bold);
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(512, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(1, 40);
            this.label2.TabIndex = 5;
            this.label2.Text = "After Point";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(80)))), ((int)(((byte)(80)))), ((int)(((byte)(80)))));
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Font = new System.Drawing.Font("Consolas", 20F, System.Drawing.FontStyle.Bold);
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(503, 40);
            this.label1.TabIndex = 4;
            this.label1.Text = "Before Point";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // tbx_BefData
            // 
            this.tbx_BefData.BackColor = System.Drawing.Color.White;
            this.tbx_BefData.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbx_BefData.Font = new System.Drawing.Font("Consolas", 15F, System.Drawing.FontStyle.Bold);
            this.tbx_BefData.ForeColor = System.Drawing.Color.Black;
            this.tbx_BefData.Location = new System.Drawing.Point(3, 83);
            this.tbx_BefData.Multiline = true;
            this.tbx_BefData.Name = "tbx_BefData";
            this.tbx_BefData.ReadOnly = true;
            this.tbx_BefData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbx_BefData.Size = new System.Drawing.Size(503, 581);
            this.tbx_BefData.TabIndex = 8;
            // 
            // FrmInspView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(510, 667);
            this.Controls.Add(this.tableLayoutPanel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "FrmInspView";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "InspView";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lbl_BefLotID;
        private System.Windows.Forms.Label lbl_AftLotID;
        private System.Windows.Forms.TextBox tbx_AftData;
        private System.Windows.Forms.TextBox tbx_BefData;


    }
}