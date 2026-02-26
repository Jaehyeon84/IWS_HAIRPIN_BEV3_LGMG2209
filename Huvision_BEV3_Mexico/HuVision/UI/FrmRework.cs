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
    public partial class FrmRework : Form
    {
        public event EventHandler ApplyEvent;

        public FrmRework()
        {
            InitializeComponent();

            for (int i = 1; i < IniManagement.Instance().inspectionModel.WeldAfterInspect_NumberofSlots + 1; i++)
            {
                UcRework ucRework = new UcRework();
                if (IniManagement.Instance().iniModel.Process_Line != 2)
                    ucRework.HideSlot();

                ucRework.GroupName = i.ToString();

                flowLayoutPanel.Controls.Add(ucRework);
            }
        }

        private void btn_Apply_Click(object sender, EventArgs e)
        {
            List<byte> reworkDataList = new List<byte>();

            foreach(UcRework ucRework in flowLayoutPanel.Controls)
            {
                bool[] tmpArr = ucRework.GetData(IniManagement.Instance().iniModel.Process_Line);
                foreach(bool b in tmpArr)
                {
                    if (b)
                        reworkDataList.AddRange(BitConverter.GetBytes((short)RESPONSE.NG));
                    else
                        reworkDataList.AddRange(BitConverter.GetBytes((short)RESPONSE.OK));
                }
            }

            if (ApplyEvent != null)
                ApplyEvent(reworkDataList, null);
        }
    }
}
