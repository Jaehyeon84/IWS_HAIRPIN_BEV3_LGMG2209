using HuVision.Model;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HuVision.UI
{
    public partial class FrmVerifyConfig : Form
    {
        public event EventHandler SaveClicked;

        private IniManagement iniManagement;

        public FrmVerifyConfig()
        {
            InitializeComponent();

            iniManagement = IniManagement.Instance();

            cb_UseR.Checked = iniManagement.iniModel.Verify_UseR;
            tbx_Threshold.Text = iniManagement.iniModel.Verify_RThreshold.ToString();

            cb_UseJump.Checked = iniManagement.iniModel.Verify_UseJump;
            tbx_JumpMin.Text = iniManagement.iniModel.Verify_JumpMin.ToString();
            tbx_JumpMax.Text = iniManagement.iniModel.Verify_JumpMax.ToString();
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            try
            {
                iniManagement.iniModel.Verify_UseR = cb_UseR.Checked;
                iniManagement.iniModel.Verify_RThreshold = double.Parse(tbx_Threshold.Text);

                iniManagement.iniModel.Verify_UseJump = cb_UseJump.Checked;
                iniManagement.iniModel.Verify_JumpMin = double.Parse(tbx_JumpMin.Text);
                iniManagement.iniModel.Verify_JumpMax = double.Parse(tbx_JumpMax.Text);
            }
            catch
            {
                MessageBox.Show("Check Value");
                return;
            }

            iniManagement.SaveIniModel();            

            if (SaveClicked != null)
                SaveClicked(null, null);
        }

        private void tbx_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsDigit(e.KeyChar)) && e.KeyChar != '.' && e.KeyChar != 8 && e.KeyChar != '-')
                e.Handled = true;
        }
    }
}
