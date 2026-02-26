using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using MIS.Common;

namespace MIS
{
    public partial class ucStatus : UserControl
    {
        private static Image toggleOn = Properties.Resources.Toggle_on;
        private static Image toggleOff = Properties.Resources.Toggle_off;

        public ucStatus()
        {
            InitializeComponent();
        }

        public void UpdatePLC(byte[] pcData, byte[] PLCData)
        {
            if (pcData == null || PLCData == null)
                return;

            //pcData [4]    
            //[0],[1] = 8bt * 2 = 16 = D30500
            //[2],[3] = 8bt * 2 = 16 = D30501

            //PC <-> PLC 간 Link Test 및 배출 보고
            //PC
            //BitArray bitArr = new BitArray(pcData);
            BitArray bitArr = new BitArray(new byte[] { pcData[0], pcData[1] });

            UpdatePb(pictureBox_PC_LinkTest, bitArr[0]);

            BitArray bitArrPCRP = new BitArray(new byte[] { pcData[2], pcData[3] });
            UpdatePb(pictureBox_PC_Input_A, bitArrPCRP[0]);
            UpdatePb(pictureBox_PC_Output_A, bitArrPCRP[1]);
            /*UpdatePb(pictureBox_PC_Input_B, bitArrPCRP[4]);
            UpdatePb(pictureBox_PC_Output_B, bitArrPCRP[5]);*/

            //PLC
            BitArray bitArrPLC = new BitArray(new byte[] { PLCData[0], PLCData[1] });
            UpdatePb(pictureBox_PLC_LinkTest, bitArrPLC[0]);

            BitArray bitArrPLCRP = new BitArray(new byte[] { PLCData[2], PLCData[3] });
            UpdatePb(pictureBox_PLC_Input_A, bitArrPLCRP[0]);
            UpdatePb(pictureBox_PLC_Output_A, bitArrPLCRP[1]);
            /*UpdatePb(pictureBox_PLC_Input_B, bitArrPLCRP[4]);
            UpdatePb(pictureBox_PLC_Output_B, bitArrPLCRP[5]);*/
        }

        public void UpdatePLCStatus(bool bLinkTest, bool bInputA, bool bOutputA, bool bInputB, bool bOutputB, 
            bool bLinkTestPC, bool bInputARes, bool bOutputARes, bool bInputBRes, bool bOutputBRes)
        {
            // PC -> PLC
            UpdatePb(pictureBox_PC_LinkTest, bLinkTestPC);

            UpdatePb(pictureBox_PC_Input_A, bInputARes);
            UpdatePb(pictureBox_PC_Output_A, bOutputARes);
            /*UpdatePb(pictureBox_PC_Input_B, bInputBRes);
            UpdatePb(pictureBox_PC_Output_B, bOutputBRes);*/

            // PLC -> PC
            UpdatePb(pictureBox_PLC_LinkTest, bLinkTest);

            UpdatePb(pictureBox_PLC_Input_A, bInputA);
            UpdatePb(pictureBox_PLC_Output_A, bOutputA);
            /*UpdatePb(pictureBox_PLC_Input_B, bInputB);
            UpdatePb(pictureBox_PLC_Output_B, bOutputB);*/
        }

        /// <summary>
        /// GMES 통신 상태 값 A, B 지그 분류
        /// </summary>
        /// <param name="isAJig"></param>
        /// <param name="bPCEventRpSend"></param>
        /// <param name="bGMESEventRPACK"></param>
        /// <param name="bGMESProdocSend"></param>
        /// <param name="bPCProdocACK"></param>
        /// <param name="bPCInsptSend"></param>
        /// <param name="bGMESInsptACK"></param>
        public void UpdateGMES(bool isAJig, bool bPCEventRpSend, bool bGMESEventRPACK, bool bGMESProdocSend, bool bPCProdocACK, bool bPCInsptSend, bool bGMESInsptACK)
        {
            if (isAJig)
            {
                UpdatePb(pictureBox_PC_EventRp_A, bPCEventRpSend);
                UpdatePb(pictureBox_GMES_EventRpACK_A, bGMESEventRPACK);
                UpdatePb(pictureBox_GMES_ProdocData_A, bGMESProdocSend);
                UpdatePb(pictureBox_PC_ProdocDataACK_A, bPCProdocACK);
                UpdatePb(pictureBox_PC_InsptDataSend_A, bPCInsptSend);
                UpdatePb(pictureBox_GMES_InsptDataACK_A, bGMESInsptACK);
            }
            else
            {
                /*UpdatePb(pictureBox_PC_EventRp_B, bPCEventRpSend);
                UpdatePb(pictureBox_GMES_EventRpACK_B, bGMESEventRPACK);
                UpdatePb(pictureBox_GMES_ProdocData_B, bGMESProdocSend);
                UpdatePb(pictureBox_PC_ProdocDataACK_B, bPCProdocACK);
                UpdatePb(pictureBox_PC_InsptDataSend_B, bPCInsptSend);
                UpdatePb(pictureBox_GMES_InsptDataACK_B, bGMESInsptACK);*/

            }
        }

        public void UpdateGMESStatus(bool bGMESLink, bool bPCLink, bool bDateSend, bool bDateACK, bool bEQPID, bool bEQPIDReq)
        {
            UpdatePb(pictureBox_GMES_LinkTest, bGMESLink);
            UpdatePb(pictureBox_GMEStoPC_LinkTest, bPCLink);
            UpdatePb(pictureBox_GMES_DateSend, bDateSend);
            UpdatePb(pictureBox_PC_DateACK, bDateACK);
            UpdatePb(pictureBox_GMES_EQPID, bEQPID);
            UpdatePb(pictureBox_PC_EQPID, bEQPIDReq);
        }

        private void UpdatePb(PictureBox pb, bool result)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate ()
                {
                    pb.Image = result == true ? toggleOn : toggleOff;
                }));
            }
            else
            {
                pb.Image = result == true ? toggleOn : toggleOff;
            }
        }

        /// <summary>
        /// Alarm 내용 사용자에게 알리기 위함
        /// </summary>
        /// <param name="strAlarm">Alarm 내용</param>
        /// <param name="isOK">true = 정상, false = 비정상</param>
        /// <param name="strJigType">Jig Type</param>
        /// <summary>        
        public void UpdateAlarm(string strAlarm, bool isOK, string strJigType)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate ()
                {
                    if (strJigType == eJigType.JigA.ToString())
                        textBox_AlarmMSG_A.Text = strAlarm;
                    /*else if (strJigType == eJigType.JigB.ToString())
                        textBox_AlarmMSG_B.Text = strAlarm;*/

                    if (strJigType == eJigType.JigA.ToString())
                    {
                        if (isOK)
                        {
                            tableLayoutPanel_Alarm_A.BackColor = Color.Green;
                            textBox_AlarmMSG_A.BackColor = Color.Green;
                            textBox_AlarmMSG_A.Dock = System.Windows.Forms.DockStyle.Fill;  // sjyi 2024.05.21 진행 상황 표기를 위해, 변경 
                            textBox_AlarmMSG_A.Font = new System.Drawing.Font("Consolas", 10F); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경
                        }
                        else
                        {
                            tableLayoutPanel_Alarm_A.BackColor = Color.Red;
                            textBox_AlarmMSG_A.BackColor = Color.Red;
                            textBox_AlarmMSG_A.Dock = System.Windows.Forms.DockStyle.Fill;
                            textBox_AlarmMSG_A.Font = new System.Drawing.Font("Consolas", 10F);
                        }
                    }
                    /*else if (strJigType == eJigType.JigB.ToString())
                    {
                        if (isOK)
                        {
                            tableLayoutPanel_Alarm_B.BackColor = Color.Green;
                            textBox_AlarmMSG_B.BackColor = Color.Green;
                            textBox_AlarmMSG_B.Dock = System.Windows.Forms.DockStyle.None;
                            textBox_AlarmMSG_B.Font = new System.Drawing.Font("Consolas", 14.25F);
                        }
                        else
                        {
                            tableLayoutPanel_Alarm_B.BackColor = Color.Red;
                            textBox_AlarmMSG_B.BackColor = Color.Red;
                            textBox_AlarmMSG_B.Dock = System.Windows.Forms.DockStyle.Fill;
                            textBox_AlarmMSG_B.Font = new System.Drawing.Font("Consolas", 10F);
                        }
                    }*/
                }));
            }
            else
            {
                if (strJigType == eJigType.JigA.ToString())
                    textBox_AlarmMSG_A.Text = strAlarm;
                /*else if (strJigType == eJigType.JigB.ToString())
                    textBox_AlarmMSG_B.Text = strAlarm;*/

                if (strJigType == eJigType.JigA.ToString())
                {
                    if (isOK)
                    {
                        tableLayoutPanel_Alarm_A.BackColor = Color.Green;
                        textBox_AlarmMSG_A.BackColor = Color.Green;
                        textBox_AlarmMSG_A.Dock = System.Windows.Forms.DockStyle.None;
                        textBox_AlarmMSG_A.Font = new System.Drawing.Font("Consolas", 14.25F);
                    }
                    else
                    {
                        tableLayoutPanel_Alarm_A.BackColor = Color.Red;
                        textBox_AlarmMSG_A.BackColor = Color.Red;
                        textBox_AlarmMSG_A.Dock = System.Windows.Forms.DockStyle.Fill;
                        textBox_AlarmMSG_A.Font = new System.Drawing.Font("Consolas", 11F);
                    }
                }
                /*else if (strJigType == eJigType.JigB.ToString())
                {
                    if (isOK)
                    {
                        tableLayoutPanel_Alarm_B.BackColor = Color.Green;
                        textBox_AlarmMSG_B.BackColor = Color.Green;
                        textBox_AlarmMSG_B.Dock = System.Windows.Forms.DockStyle.None;
                        textBox_AlarmMSG_B.Font = new System.Drawing.Font("Consolas", 14.25F);
                    }
                    else
                    {
                        tableLayoutPanel_Alarm_B.BackColor = Color.Red;
                        textBox_AlarmMSG_B.BackColor = Color.Red;
                        textBox_AlarmMSG_B.Dock = System.Windows.Forms.DockStyle.Fill;
                        textBox_AlarmMSG_B.Font = new System.Drawing.Font("Consolas", 11F);
                    }
                }*/
            }
        }

        public void UpdateProductInEquip(bool bInJigA, bool bInJigB)
        {
            UpdatePb(pictureBox_JigA, bInJigA);
            //UpdatePb(pictureBox_JigB, bInJigB);
        }

        public void UpdateRepairMode(bool bRepair)
        {
            if(bRepair)
            {
                label6.Text = "GMES Interface System(#95)";
            }
            else
            {
                label6.Text = "GMES Interface System";
            }
        }
    }
}
