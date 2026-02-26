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
    public partial class FrmConfig : Form
    {
        public event EventHandler SaveClicked;

        private IniManagement iniManagement;

        public FrmConfig()
        {
            InitializeComponent();

            iniManagement = IniManagement.Instance();

            tbx_77_Slot.Text = iniManagement.iniModel.Model77_Slot.ToString();
            tbx_77_Layer.Text = iniManagement.iniModel.Model77_Layer.ToString();
            tbx_77_Angle.Text = iniManagement.iniModel.Model77_Angle.ToString();
            tbx_77_Range.Text = iniManagement.iniModel.Model77_Range.ToString();
            tbx_77_XOffset.Text = iniManagement.iniModel.Model77_XOffSet.ToString();
            tbx_77_YOffset.Text = iniManagement.iniModel.Model77_YOffSet.ToString();

            tbx_76_Slot.Text = iniManagement.iniModel.Model76_Slot.ToString();
            tbx_76_Layer.Text = iniManagement.iniModel.Model76_Layer.ToString();
            tbx_76_Angle.Text = iniManagement.iniModel.Model76_Angle.ToString();
            tbx_76_Range.Text = iniManagement.iniModel.Model76_Range.ToString();
            tbx_76_XOffset.Text = iniManagement.iniModel.Model76_XOffSet.ToString();
            tbx_76_YOffset.Text = iniManagement.iniModel.Model76_YOffSet.ToString();
        }

        private void btn_Save_Click(object sender, EventArgs e)
        {
            try
            {
                iniManagement.iniModel.Model77_Slot = int.Parse(tbx_77_Slot.Text);
                iniManagement.iniModel.Model77_Layer = int.Parse(tbx_77_Layer.Text);

                iniManagement.iniModel.Model77_Angle = float.Parse(tbx_77_Angle.Text);
                iniManagement.iniModel.Model77_Range = float.Parse(tbx_77_Range.Text);
                iniManagement.iniModel.Model77_XOffSet = float.Parse(tbx_77_XOffset.Text);
                iniManagement.iniModel.Model77_YOffSet = float.Parse(tbx_77_YOffset.Text);

                iniManagement.iniModel.Model76_Slot = int.Parse(tbx_76_Slot.Text);
                iniManagement.iniModel.Model76_Layer = int.Parse(tbx_76_Layer.Text);

                iniManagement.iniModel.Model76_Angle = float.Parse(tbx_76_Angle.Text);
                iniManagement.iniModel.Model76_Range = float.Parse(tbx_76_Range.Text);
                iniManagement.iniModel.Model76_XOffSet = float.Parse(tbx_76_XOffset.Text);
                iniManagement.iniModel.Model76_YOffSet = float.Parse(tbx_76_YOffset.Text);
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
