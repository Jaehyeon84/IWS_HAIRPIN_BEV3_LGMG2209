using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HuVision.UI
{
    public partial class UcRework : UserControl
    {
        public string GroupName
        {
            get { return groupBox.Text; }
            set { groupBox.Text = value; }
        }

        public UcRework()
        {
            InitializeComponent();
        }

        public void HideSlot()
        {
            this.Height = 60;
            this.cb_3.Visible = false;
            this.cb_4.Visible = false;
        }

        public bool[] GetData(int line)
        {
            if (line == 2)
                return new bool[] { cb_1.Checked, cb_2.Checked, cb_3.Checked, cb_4.Checked };
            else
                return new bool[] { cb_1.Checked, cb_2.Checked };
        }
    }
}
