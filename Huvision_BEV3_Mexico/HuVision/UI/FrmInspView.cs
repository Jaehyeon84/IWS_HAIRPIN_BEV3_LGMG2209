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
    public partial class FrmInspView : Form
    {
        public FrmInspView()
        {
            InitializeComponent();
        }

        public void SetBef(string lotID, string data)
        {
            lbl_BefLotID.Text = lotID;
            tbx_BefData.Text = data;
        }

        public void SetAft(string lotID, string data)
        {
            lbl_AftLotID.Text = lotID;
            tbx_AftData.Text = data;
        }
    }
}
