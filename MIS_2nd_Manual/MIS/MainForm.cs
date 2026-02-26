using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Timers;
using System.Threading;
using HubisCommonControl;  // 공통 클래스 사용하기 위해 선언(Log, Socket 등)
using MIS.Common;
using MIS.Communication;

namespace MIS
{
    public partial class MainForm : Form
    {
        // Config
        public ConfigINI m_config = new ConfigINI();

        // PLC Interface
        PLCSocket m_PLCSocket = null;

        // GMES Interface
        GMESSocket m_GMESSocket = null;

        // Sequence Thread
        private Thread SequenceThread;
        private bool bSeqThreadRun = false;

        private eEuipmentSeq m_eEqpSeq = eEuipmentSeq.EQP_INIT;

        private System.Timers.Timer m_LnkTimer = new System.Timers.Timer();

        // Communication State("O" : Online, "F" : Offline)
        protected string m_strEQPState = "0";

        private int m_nInitSeqID = 0; // 초기화 Sequence Index

        // sjyi 2024.01.15 이전장비 검사 결과 변수
        List<PreInspectResult> m_lsPreInspResult = new List<PreInspectResult>();

        private int m_nPCLinkCnt = 0;
        private bool m_bLinkPLC = false;
        private DateTime m_LinkStart;
        private DateTime m_LinkEnd;

        public MainForm()
        {
            InitializeComponent();

            // Initialize InterfaceClass
            InitializeHW();

            // Initialize UI Controls
            InitializeCtrl();
        }

        private void MainFormClosing(object sender, FormClosingEventArgs e)
        {
            if (MessageBox.Show("Do you want to shut down the GMES System?", "GMES System", MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                try
                {
                    TerminateHW();
                }
                catch { }

                Application.ExitThread();
                Environment.Exit(0);
            }
            else
            {
                e.Cancel = true;

                // Hide Window & Set SystemTray Icon
                this.WindowState = FormWindowState.Minimized;
            }

        }

        public void InitializeHW()
        {
            LogManager.WriteLog(LogType.Information, "MES Interface System is Started ========================");

            // Config Read
            string strPath = Application.StartupPath + "\\MISConfig.ini";
            string strExe = Application.StartupPath + "\\";
            m_config.InIPath = strPath;
            m_config.ReadConfig();
            m_config.WriteConfig();

            // PLC 및 MES Socket 생성
            m_PLCSocket = new PLCSocket();
            m_GMESSocket = new GMESSocket();
            m_PLCSocket.Intialize();
            m_GMESSocket.Intialize();
            m_bLinkPLC = m_PLCSocket.GetLink(); // Link Test 초기값 가져오기
            m_LinkStart = DateTime.Now; // Timeout 시작 시간 변경

            // Seq Thread Start
            SeqThreadStart();

            // PLC Link Timer 시작(250 ms)
            m_LnkTimer.Interval = 250;
            m_LnkTimer.Elapsed += new System.Timers.ElapsedEventHandler(Timer_Elapsed);
            m_LnkTimer.Start();
        }

        public void InitializeCtrl()
        {
            // Hide Window & Set SystemTray Icon
            this.WindowState = FormWindowState.Minimized;
            notifyIcon1.Visible = true;
            this.Hide();
            this.ShowInTaskbar = false;

            // Maint Button Invisible
            button_PLC_InputA.Visible = false;
            button_PLC_outputA.Visible = false;
            button_PLC_ErrorA.Visible = false;

            /*button_PLC_InputB.Visible = false;
            button_PLC_outputB.Visible = false;
            button_PLC_ErrorB.Visible = false;*/
            btn_Clear_GMES.Visible = false;
            btn_Clear_GMESA.Visible = false;
            //btn_Clear_GMESB.Visible = false;
            buttonClearPLC.Visible = false;

            ucMISStatus.UpdateRepairMode(m_config.m_bRepairWeld);
        }

        public void TerminateHW()
        {
            // Timer 종료
            m_LnkTimer.Stop();

            // Stop Seq Thread
            SeqThreadStop();

            // PLC 및 MES Socket 삭제
            if(m_PLCSocket != null)
            {
                m_PLCSocket.Terminate();
            }

            if (m_GMESSocket != null)
            {
                m_GMESSocket.Terminate();
                m_GMESSocket.isClose = true;
                m_GMESSocket.Close();
                m_GMESSocket = null;
            }

            LogManager.WriteLog(LogType.Information, "MES Interface System is Exited =========================");
        }

        delegate void TimerEventFiredDelegate();

        void Timer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            BeginInvoke(new TimerEventFiredDelegate(TimerEvent));
        }

        private void TimerEvent()
        {
            if(m_PLCSocket.GetIsConnected())
            {
                if (m_nPCLinkCnt < 1)
                {
                    m_nPCLinkCnt++;
                }
                else // 250 msec * 2(500 msec 마다 Link 신호 반전)
                {
                    m_nPCLinkCnt = 0;
                    bool bLink = m_PLCSocket.GetLinkTestRes();
                    m_PLCSocket.SetLink(!bLink);
                }
            }

            // 화면 갱신
            UpdateIFStatus();

            // PLC Link Test Timeout Check
            CheckLinkPLC();
        }

        private void UpdateIFStatus()
        {
            // AJig(MES) Event Report Send, Product Data Send, Inspection Data Send
            ucMISStatus.UpdateGMES(true,
                m_GMESSocket.GetMESStatusJigA().bEQP_EERS,
                m_GMESSocket.GetMESStatusJigA().bGMES_EERS_R,
                m_GMESSocket.GetMESStatusJigA().bGMES_EPDS,
                m_GMESSocket.GetMESStatusJigA().bEQP_EPDS_R,
                m_GMESSocket.GetMESStatusJigA().bEQP_EIDS,
                m_GMESSocket.GetMESStatusJigA().bGMES_EIDS_R
                );

            // BJig(MES) Event Report Send, Product Data Send, Inspection Data Send
            ucMISStatus.UpdateGMES(false,
                m_GMESSocket.GetMESStatusJigB().bEQP_EERS,
                m_GMESSocket.GetMESStatusJigB().bGMES_EERS_R,
                m_GMESSocket.GetMESStatusJigB().bGMES_EPDS,
                m_GMESSocket.GetMESStatusJigB().bEQP_EPDS_R,
                m_GMESSocket.GetMESStatusJigB().bEQP_EIDS,
                m_GMESSocket.GetMESStatusJigB().bGMES_EIDS_R
                );

            // MES Common(Link, Date Time, Are You There)
            ucMISStatus.UpdateGMESStatus(
                m_GMESSocket.m_bLinkTest,
                m_GMESSocket.m_bLinkTestAck,
                m_GMESSocket.m_bGMES_EDTS,
                m_GMESSocket.m_bEQP_EDTS_R,
                m_GMESSocket.m_bGMES_EAYT_R,
                m_GMESSocket.m_bEQP_EAYT
                );

            // PLC Status
            ucMISStatus.UpdatePLCStatus(
                m_PLCSocket.GetLink(),
                m_PLCSocket.GetInputA(),
                m_PLCSocket.GetOutputA(),
                m_PLCSocket.GetInputB(),
                m_PLCSocket.GetOutputB(),
                m_PLCSocket.GetLinkTestRes(),
                m_PLCSocket.GetInputARes(),
                m_PLCSocket.GetOutputARes(),
                m_PLCSocket.GetInputBRes(),
                m_PLCSocket.GetOutputBRes()
                );

            // "Product In Jig" flag
            ucMISStatus.UpdateProductInEquip(
                m_PLCSocket.GetProductInJIGA(),
                m_PLCSocket.GetProductInJIGB()
                );

        }

        protected void SeqThreadStart()
        {
            SequenceThread = new Thread(new ThreadStart(SeqThreadProcess));
            bSeqThreadRun = true;
            SequenceThread.Start();
        }

        protected void SeqThreadStop()
        {
            bSeqThreadRun = false;
            SequenceThread.Abort();
            SequenceThread = null;
        }

        // Sequence Thread Function
        protected void SeqThreadProcess()
        {
            while (bSeqThreadRun)
            {
                SeqProcess();

                Thread.Sleep(100);
            }
        }

        // Main Sequence Function
        protected void SeqProcess()
        {
            // Get Current Sequence
            int nEqpMode = GetCurrentSeq();

            switch(nEqpMode)
            {
                case (int)eEuipmentSeq.EQP_INIT:    // 초기화
                    SeqProcInit();
                    break;
                case (int)eEuipmentSeq.EQP_OFFLINE: // OFFLINE(통신 끊김, 통신 연결 될 때까지 대기)
                    SeqProcOffline();
                    break;
                case (int)eEuipmentSeq.EQP_ONLINE:  // ONLINE(투입 배출 보고)
                    SeqProcOnline();
                    break;
            }
        }

        protected int GetCurrentSeq()
        {
            return (int)m_eEqpSeq;
        }

        protected void SetCurrentSeq(int nEqpMode)
        {
            m_eEqpSeq = (eEuipmentSeq)nEqpMode;
            LogManager.WriteLog(LogType.Information, "[SEQ] Change To " + m_eEqpSeq.ToString());
        }

        // 초기화 프로세스
        protected void SetCurrentInitSeq(int nSeqNo)
        {
            m_nInitSeqID = nSeqNo;
        }

        protected void SeqProcInit()
        {
            // 연결 상태 확인
            if (m_GMESSocket.IsConnected == false && m_PLCSocket.GetIsConnected() == false)
            {
                // 연결이 안되어 있으면 OFFLINE 상태로 변경하고 종료
                m_GMESSocket.ClearAllFlag();
                SetCurrentInitSeq(0);

                SetCurrentSeq((int)eEuipmentSeq.EQP_OFFLINE);

                // Set Status to OFFLINE
                ucMISStatus.UpdateAlarm("OFFLINE", false, eJigType.JigA.ToString());
                ucMISStatus.UpdateAlarm("OFFLINE", false, eJigType.JigB.ToString());

                return;
            }

            // 초기화 시퀀스 
            switch (m_nInitSeqID)
            {
                case 0: // Send EAYT
                    if(m_GMESSocket.m_bEQP_EAYT == false && m_GMESSocket.m_bGMES_EAYT_R == false)
                    {
                        // Send EAYT(Are You There) Request
                        m_GMESSocket.SendEAYT();    
                    }
                    else if(m_GMESSocket.m_bGMES_EAYT_R == true)
                    {
                        SetCurrentInitSeq(1);
                    }
                    break;
                case 1: // Send EEMR
                    if (m_GMESSocket.m_bEQP_EEMR == false && m_GMESSocket.m_bGMES_EEMR_R == false)
                    {
                        // Send EEMR(Equipment Mode Report)
                        m_GMESSocket.SendEEMR(m_GMESSocket.GetEquipInfo().EQPID, m_strEQPState);    
                    }
                    else if (m_GMESSocket.m_bGMES_EEMR_R == true)
                    {
                        SetCurrentInitSeq(2);
                    }
                    break;
                case 2: // Check Receive ACK
                    if (m_GMESSocket.m_bGMES_EAYT_R == true && m_GMESSocket.m_bGMES_EEMR_R == true)
                    {
                        // Change Sequence Online
                        SetCurrentSeq((int)eEuipmentSeq.EQP_ONLINE);
                    }
                    break;
            }

            SeqProcGMES();
        }

        // 온라인 프로세스
        protected void SeqProcOnline()
        {
            // Watch PLC Event 
            SeqProcPLC();

            // Watch GMES Event
            SeqProcGMES();
        }

        // PLC 감시 프로세스(투입 배출 감시)
        protected void SeqProcPLC()
        {
            // 연결상태 확인 하여 끊어졌으면, 재연결 시도(재연결 시도는 PLCSocket 클래스 자체적으로 시도)
            if (m_PLCSocket.GetIsConnected() == false)
            {
                return;
            }

            // Input Event Sequence
            SeqProcInputEvent();

            // Inspect Event Sequence
            SeqProcInspectEvent();

            // Output Event Sequence
            SeqProcOutputEvent();

            // Pass Event Sequence
            SeqProcPassEvent();

            // Set InJig Flag 
            m_GMESSocket.SetProductInJig(0, m_PLCSocket.GetProductInJIGA());
            m_GMESSocket.SetProductInJig(1, m_PLCSocket.GetProductInJIGB());
        }

        // PLC 감시 - 투입 감시 
        protected void SeqProcInputEvent()
        {
            // 투입 감시(JIG A)
            if (m_PLCSocket.GetInputA() == true && m_PLCSocket.GetInputOldA() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Input Request ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Input Request ON", true, eJigType.JigA.ToString());

                // Send EERS : Event Report Send
                EAYT_R EqpInfo = m_GMESSocket.GetEquipInfo();
                m_GMESSocket.SendEERS(EqpInfo.EQPID, m_config.strCEID, m_config.strRPTID, m_PLCSocket.GetBarcodeA(), 0);
            }
            else if (m_PLCSocket.GetInputA() == false && m_PLCSocket.GetInputOldA() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Input Request OFF");

                // Set Ui to Normal
                //ucMISStatus.UpdateAlarm("[PLC->PC] Input Request OFF", true, eJigType.JigA.ToString());

                // Set Input Response(OFF)
                m_PLCSocket.SetInputARes(false);
            }
            
            // Set Old Value(for Edge Detecting)
            m_PLCSocket.SetInputOldA(m_PLCSocket.GetInputA());
            

            // 투입 감시(JIG B)
            if (m_PLCSocket.GetInputB() == true && m_PLCSocket.GetInputOldB() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Input Request ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Input Request ON", true, eJigType.JigB.ToString());

                // Send EERS : Event Report Send
                EAYT_R EqpInfo = m_GMESSocket.GetEquipInfo();
                m_GMESSocket.SendEERS(EqpInfo.EQPID, m_config.strCEID, m_config.strRPTID, m_PLCSocket.GetBarcodeB(), 1);
            }
            else if (m_PLCSocket.GetInputB() == false && m_PLCSocket.GetInputOldB() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Input Request OFF");

                // Set Ui to Normal
                //ucMISStatus.UpdateAlarm("[PLC->PC] Input Request OFF", true, eJigType.JigB.ToString());

                // Set Input Response(OFF)
                m_PLCSocket.SetInputBRes(false);
            }

            // Set Old Value(for Edge Detecting)
            m_PLCSocket.SetInputOldB(m_PLCSocket.GetInputB());
        }

        // PLC 감시 - 검사 이벤트 감시
        protected void SeqProcInspectEvent()
        {
            // 전검사 이벤트 감시 ---------------------------------------------------------------------------
            // 전검사 감시(JIG A)
            if (m_PLCSocket.GetBeforeInspectA() == true && m_PLCSocket.GetBeforeInspectARes() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Before Inspection ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Before Inspection ON", true, eJigType.JigA.ToString());

                // 검사결과 저장
                m_PLCSocket.ReadBefInspect(0, true);

                // Set Response(ON)
                m_PLCSocket.SetBeforeInspectARes(true);

            }
            else if (m_PLCSocket.GetBeforeInspectA() == false && m_PLCSocket.GetBeforeInspectARes() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Before Inspection OFF");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Before Inspection OFF", true, eJigType.JigA.ToString());

                // Set Response(OFF)
                m_PLCSocket.SetBeforeInspectARes(false);
            }

            // 전검사 감시(JIG B)
            if (m_PLCSocket.GetBeforeInspectB() == true && m_PLCSocket.GetBeforeInspectBRes() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Before Inspection ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Before Inspection ON", true, eJigType.JigB.ToString());

                // 검사결과 저장
                m_PLCSocket.ReadBefInspect(1, true);

                // Set Response(ON)
                m_PLCSocket.SetBeforeInspectBRes(true);
            }
            else if (m_PLCSocket.GetBeforeInspectB() == false && m_PLCSocket.GetBeforeInspectBRes() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Before Inspection OFF");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Before Inspection OFF", true, eJigType.JigB.ToString());

                // Set Response(OFF)
                m_PLCSocket.SetBeforeInspectBRes(false);
            }

            // 후검사 이벤트 감시 ---------------------------------------------------------------------------
            // 후검사 감시(JIG A)
            if (m_PLCSocket.GetAfterInspectA() == true && m_PLCSocket.GetAfterInspectARes() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : After Inspection ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] After Inspection ON", true, eJigType.JigA.ToString());

                // 검사결과 저장
                m_PLCSocket.ReadAfterInspect(0);

                // Set Response(ON)
                m_PLCSocket.SetAfterInspectARes(true);
            }
            else if (m_PLCSocket.GetAfterInspectA() == false && m_PLCSocket.GetAfterInspectARes() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : After Inspection OFF");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] After Inspection OFF", true, eJigType.JigA.ToString());

                // Set Response(OFF)
                m_PLCSocket.SetAfterInspectARes(false);
            }

            // 후검사 감시(JIG B)
            if (m_PLCSocket.GetAfterInspectB() == true && m_PLCSocket.GetAfterInspectBRes() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : After Inspection ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] After Inspection ON", true, eJigType.JigB.ToString());

                // 검사결과 저장
                m_PLCSocket.ReadAfterInspect(1);

                // Set Response(ON)
                m_PLCSocket.SetAfterInspectBRes(true);
            }
            else if (m_PLCSocket.GetAfterInspectB() == false && m_PLCSocket.GetAfterInspectBRes() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : After Inspection OFF");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] After Inspection OFF", true, eJigType.JigB.ToString());

                // Set Response(OFF)
                m_PLCSocket.SetAfterInspectBRes(false);
            }
        }

        // PLC 감시 - 배출 이벤트 감시
        protected void SeqProcOutputEvent()
        {
            // 배출 감시(JIG A)
            if (m_PLCSocket.GetOutputA() == true && m_PLCSocket.GetOutputOldA() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Output Request ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Output Request ON", true, eJigType.JigA.ToString());

                // 2024.02.12 jh.kim 추가 - Repair 전검사 누적(초기화 추가)
                // If Before Inspect Data's Count is Zero, then Read Inspect Data As 1st Inspection
                //if (m_PLCSocket.GetInspectData(0).Count == 0)
                if (m_PLCSocket.GetInspectData(0).Count == 0 || m_PLCSocket.GetRepairWeld() == true)
                {
                    m_PLCSocket.ReadBefInspect(0, true);
                }

                // Read Before & After Inspect Data
                m_PLCSocket.ReadBefInspect(0, false);
                m_PLCSocket.ReadAfterInspect(0);

                // Check ProductData -> 프로그램이 종료 후, 재시작 되도 Ini파일의 정보를 가지고 보고하도록 수정
                if(m_GMESSocket.CheckProductData(0) == false)
                {
                    // Ini 파일 정보 읽어와서 Product Data 저장
                    m_GMESSocket.SetProductDataExceptSubItem(0, m_config.ReadProductData(0));
                }

                // Send EIDS : Inspection Data Send
                m_GMESSocket.SendEIDS(
                    m_PLCSocket.GetInspectData(0),
                    m_PLCSocket.GetPLCModel(),
                    m_PLCSocket.GetReworkCntA(),
                    m_PLCSocket.GetBarcodeA(),
                    eJigType.JigA.ToString(),
                    m_GMESSocket.GetProductData(0),
                    m_GMESSocket.GetEquipInfo(),
                    m_PLCSocket.GetFinalJudgeA(),
                    0);

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PC->GMES] EIDS Send", true, eJigType.JigA.ToString());
            }
            else if (m_PLCSocket.GetOutputA() == false && m_PLCSocket.GetOutputOldA() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Output Request OFF");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Output Request OFF", true, eJigType.JigA.ToString());

                // Set Response(OFF)
                m_PLCSocket.SetOutputARes(false);
            }

            // Set Old Value(for Edge Detecting)
            m_PLCSocket.SetOutputOldA(m_PLCSocket.GetOutputA());


            // 배출 감시(JIG B)
            if (m_PLCSocket.GetOutputB() == true && m_PLCSocket.GetOutputOldB() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Output Request ON");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Output Request ON", true, eJigType.JigB.ToString());

                // 2024.02.12 jh.kim 추가 - Repair 전검사 누적(초기화 추가)
                // If Before Inspect Data's Count is Zero, then Read Inspect Data As 1st Inspection
                //if (m_PLCSocket.GetInspectData(0).Count == 0)
                if (m_PLCSocket.GetInspectData(1).Count == 0 || m_PLCSocket.GetRepairWeld() == true)
                {
                    m_PLCSocket.ReadBefInspect(1, true);
                }

                // Read Before & After Inspect Data
                m_PLCSocket.ReadBefInspect(1, false);
                m_PLCSocket.ReadAfterInspect(1);

                // Check ProductData -> 프로그램이 종료 후, 재시작 되도 Ini파일의 정보를 가지고 보고하도록 수정
                if (m_GMESSocket.CheckProductData(1) == false)
                {
                    // Ini 파일 정보 읽어와서 Product Data 저장
                    m_GMESSocket.SetProductDataExceptSubItem(1, m_config.ReadProductData(1));
                }

                // Send EIDS : Inspection Data Send
                m_GMESSocket.SendEIDS(
                    m_PLCSocket.GetInspectData(1),
                    m_PLCSocket.GetPLCModel(),
                    m_PLCSocket.GetReworkCntB(),
                    m_PLCSocket.GetBarcodeB(),
                    eJigType.JigB.ToString(),
                    m_GMESSocket.GetProductData(1),
                    m_GMESSocket.GetEquipInfo(),
                    m_PLCSocket.GetFinalJudgeB(),
                    1);

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PC->GMES] EIDS Send", true, eJigType.JigB.ToString());
            }
            else if (m_PLCSocket.GetOutputB() == false && m_PLCSocket.GetOutputOldB() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Output Request OFF");

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Output Request OFF", true, eJigType.JigB.ToString());

                // Set Response(OFF)
                m_PLCSocket.SetOutputBRes(false);
            }

            // Set Old Value(for Edge Detecting)
            m_PLCSocket.SetOutputOldB(m_PLCSocket.GetOutputB());
        }

        // PLC 감시 - PASS 이벤트 감시
        protected void SeqProcPassEvent()
        {
            // Pass 감시(JIG A)
            if (m_PLCSocket.GetPassA() == true && m_PLCSocket.GetPassResA() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Pass Request ON");

                // Set Response(ON)
                m_PLCSocket.SetPassResA(true);

                // Clear Errors
                m_PLCSocket.SetInputErrorA(false);
                m_PLCSocket.Set_A_Model_Error(false);
                m_PLCSocket.SetOutputErrorA(false);
                m_GMESSocket.ClearJigStatus(0);

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Pass Request", true, eJigType.JigA.ToString()); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경
            }
            else if (m_PLCSocket.GetPassA() == false && m_PLCSocket.GetPassResA() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig A : Pass Request OFF");

                // Set Response(OFF)
                m_PLCSocket.SetPassResA(false);
            }

            // Pass 감시(JIG B)
            if (m_PLCSocket.GetPassB() == true && m_PLCSocket.GetPassResB() == false)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Pass Request ON");

                // Set Response(ON)
                m_PLCSocket.SetPassResB(true);

                // Clear Errors
                m_PLCSocket.SetInputErrorB(false);
                m_PLCSocket.Set_B_Model_Error(false);
                m_PLCSocket.SetOutputErrorB(false);
                m_GMESSocket.ClearJigStatus(1);

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[PLC->PC] Pass Request", true, eJigType.JigB.ToString()); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경
            }
            else if (m_PLCSocket.GetPassB() == false && m_PLCSocket.GetPassResB() == true)
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[PLC->PC] Jig B : Pass Request OFF");

                // Set Response(OFF)
                m_PLCSocket.SetPassResB(false);
            }
        }

        // GMES Event 감시
        protected void SeqProcGMES()
        {
            // 연결상태 확인
            // 끊어지면 모든 Flag Off로 바꾸고 Offline 상태로 변경
            if (m_GMESSocket.IsConnected == false)
            {
                // 연결이 안되어 있으면 OFFLINE 상태로 변경하고 종료
                m_GMESSocket.ClearAllFlag();
                SetCurrentInitSeq(0);

                SetCurrentSeq((int)eEuipmentSeq.EQP_OFFLINE);

                // Set Status to OFFLINE
                ucMISStatus.UpdateAlarm("OFFLINE", false, eJigType.JigA.ToString());
                ucMISStatus.UpdateAlarm("OFFLINE", false, eJigType.JigB.ToString());

                return;
            }

            // 수신 메세지 확인
            string strMsgID = m_GMESSocket.GetReceiveJson();

            // 수신 메세지별 대응
            int nJigNo = -1;
            switch(strMsgID)
            {
                case "ELNT": // Link Test 
                    m_GMESSocket.RcvProcELNT();
                    break;
                case "EAYT_R": // Are You There Data Ack
                    m_GMESSocket.RcvProcEAYT_R();
                    break;
                case "EDTS": // Date and Time Send
                    m_GMESSocket.RcvProcEDTS();
                    break;
                case "EEMR_R": // Equipment Mode Report Ack
                    m_GMESSocket.RcvProcEEMR_R();
                    break;
                case "EIDS_R": // Inspection Data Send Ack
                    nJigNo = m_GMESSocket.RcvProcEIDS_R();

                    SeqProcRcvEIDS_R(nJigNo);
                    break;
                case "EERS_R": // Event Report Send Ack
                    m_GMESSocket.RcvProcEERS_R();

                    SeqProcRcvEERS_R();
                    break;
                case "EPDS": // Product Data Send
                    nJigNo = m_GMESSocket.RcvProcEPDS();

                    SeqProcRcvEPDS(nJigNo);
                    break;
            }
        }

        // GMES EERS_R 메시지 수신시 동작
        private void SeqProcRcvEERS_R()
        {
            // Get JigType, Ack & Reason
            eJigType JigType = m_GMESSocket.GetEERS_R().nJigType;
            string strReason = m_GMESSocket.GetEERS_R().REASON;
            string strACK = m_GMESSocket.GetEERS_R().ACK;

            if(JigType == eJigType.JigA)
            {
                SeqProcRcvEERS_R_JigA(strACK, strReason);
            }
            else if(JigType == eJigType.JigB)
            {
                SeqProcRcvEERS_R_JigB(strACK, strReason);
            }
        }

        private void SeqProcRcvEERS_R_JigA(string strACK, string strReason)
        {
            if (strACK != "0") // Not Accepted
            {
                // Logging
                LogManager.WriteLog(LogType.Warning, "[" + eJigType.JigA.ToString() + "] Event Rerport Not Accepted - " + strReason);

                // UI Update(Not Accepted)
                ucMISStatus.UpdateAlarm(strReason, false, eJigType.JigA.ToString());

                // Set Input Error
                m_PLCSocket.SetInputErrorA(true);

                // Set Input Response to PLC
                if (m_PLCSocket.GetInputA()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetInputARes(true);
                }
            }
            else // Accepted
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[" + eJigType.JigA.ToString() + "] Event Rerport Accepted");

                // UI Update(normal)
                ucMISStatus.UpdateAlarm("[GMES->PC] EERS_R : Event Report Accepted", true, eJigType.JigA.ToString()); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경

                // Clear Input Error
                m_PLCSocket.SetInputErrorA(false);
            }
        }

        private void SeqProcRcvEERS_R_JigB(string strACK, string strReason)
        {
            if (strACK != "0") // Not Accepted
            {
                // Logging
                LogManager.WriteLog(LogType.Warning, "[" + eJigType.JigB.ToString() + "] Event Rerport Not Accepted - " + strReason);

                // UI Update(Not Accepted)
                ucMISStatus.UpdateAlarm(strReason, false, eJigType.JigB.ToString());

                // Set Input Error
                m_PLCSocket.SetInputErrorB(true);

                // Set Input Response to PLC
                if (m_PLCSocket.GetInputB()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetInputBRes(true);
                }
            }
            else // Accepted
            {
                // Logging
                LogManager.WriteLog(LogType.Information, "[" + eJigType.JigB.ToString() + "] Event Rerport Accepted");

                // UI Update(normal)
                ucMISStatus.UpdateAlarm("[GMES->PC] EERS_R : Event Report Accepted", true, eJigType.JigB.ToString()); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경

                // Clear Input Error
                m_PLCSocket.SetInputErrorB(false);
            }
        }

        // GMES EPDS 메시지 수신시 동작
        private void SeqProcRcvEPDS(int nJigNo)
        {
            // Logging
            LogManager.WriteLog(LogType.Information, "EPDS Received Jig No : " + nJigNo.ToString());
            
            // Write to Ini File
            m_config.WriteProductData(nJigNo, m_GMESSocket.GetProductData(nJigNo));

            // sjyi 2024.01.15 추가(전검사 데이터 Parsing 및 PLC에 저장)
            SetPreInspectResult(nJigNo);

            if (nJigNo == 0) // Jig A
            {
                SeqProcRcvEPDS_JigA();
            }
            else if(nJigNo == 1) // Jig B
            {
                SeqProcRcvEPDS_JigB();
            }
            else // Don't Know Jig No -> Input Bit로 확인
            {
                if(m_PLCSocket.GetInputA())
                {
                    SeqProcRcvEPDS_JigA();

                    m_GMESSocket.SetEPDS_Flag(0, true);
                }
                else if (m_PLCSocket.GetInputB())
                {
                    SeqProcRcvEPDS_JigB();

                    m_GMESSocket.SetEPDS_Flag(1, true);
                }
            }
        }

        private void SeqProcRcvEPDS_JigA()
        {
            int nJigNo = 0;

            // Check Stator Model
            bool bCheck = m_PLCSocket.CheckStatorModel(m_GMESSocket.GetProductData(nJigNo).strStatorType);

            if (bCheck == false) // Stator Model이 안맞음.
            {
                // Logging
                LogManager.WriteLog(LogType.Warning, "[Jig A] Model Error - MES : " + m_GMESSocket.GetProductData(nJigNo).strStatorType + ", PLC : " + m_PLCSocket.GetPLCModel());

                // Set Model Error Flag & Display Error Msg
                m_PLCSocket.Set_A_Model_Error(true);
                ucMISStatus.UpdateAlarm("Model Error", false, eJigType.JigA.ToString());

                // Set Input Response ON(PLC) -- jigA
                if (m_PLCSocket.GetInputA()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetInputARes(true);
                }
            }
            else // Stator Model 정상
            {
                // Clear Model Error Flag & Display Status to Normal
                m_PLCSocket.Set_A_Model_Error(false);
                //ucMISStatus.UpdateAlarm("EPDS(Jig A)", true, eJigType.JigA.ToString());

                // Set Input Response ON(PLC) -- jigA
                if (m_PLCSocket.GetInputA())
                {
                    m_PLCSocket.SetInputARes(true);
                }

                // Product Logging
                LogManager.ProductWriteLog(LogType.Information, "The product has been enterd to Jig A. Model = [" + m_PLCSocket.GetPLCModel() + "], BarCode = [" + m_PLCSocket.GetBarcodeA() + "]", m_config.ProductLogPath);
            }
        }

        private void SeqProcRcvEPDS_JigB()
        {
            int nJigNo = 1;

            // Check Stator Model
            bool bCheck = m_PLCSocket.CheckStatorModel(m_GMESSocket.GetProductData(nJigNo).strStatorType);

            // sjyi 2024.01.15 추가(전검사 데이터 Parsing 및 PLC에 저장)
            SetPreInspectResult(nJigNo);

            if (bCheck == false) // Stator Model이 안맞음.
            {
                // Logging
                LogManager.WriteLog(LogType.Warning, "[Jig B] Model Error - MES : " + m_GMESSocket.GetProductData(nJigNo).strStatorType + ", PLC : " + m_PLCSocket.GetPLCModel());

                // Set Model Error Flag & Display Error Msg
                m_PLCSocket.Set_B_Model_Error(true);
                ucMISStatus.UpdateAlarm("Model Error", false, eJigType.JigB.ToString());

                // Set Input Response ON(PLC) -- jigB
                if (m_PLCSocket.GetInputB()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetInputBRes(true);
                }
            }
            else // Stator Model 정상
            {
                // Clear Model Error Flag & Display Status to Normal
                m_PLCSocket.Set_B_Model_Error(false);
                //ucMISStatus.UpdateAlarm("EPDS(Jig B)", true, eJigType.JigB.ToString());

                // Set Input Response ON(PLC) -- jigB
                if (m_PLCSocket.GetInputB())
                {
                    m_PLCSocket.SetInputBRes(true);
                }

                // Product Logging
                LogManager.ProductWriteLog(LogType.Information, "The product has been enterd to Jig B. Model = [" + m_PLCSocket.GetPLCModel() + "], BarCode = [" + m_PLCSocket.GetBarcodeB() + "]", m_config.ProductLogPath);
            }
        }

        private void SeqProcRcvEIDS_R(int nJigNo)
        {
            // Logging
            LogManager.WriteLog(LogType.Information, "EIDS_R Received Jig No : " + nJigNo.ToString());

            // Get Ack & Reason
            string strACK = m_GMESSocket.GetEIDS_R().ACK;
            string strReason = m_GMESSocket.GetEIDS_R().REASON;

            if(nJigNo == 0) // Jig A
            {
                SeqProcRcvEIDS_R_JigA(strACK, strReason);
            }
            else if(nJigNo == 1) // Jig B
            {
                SeqProcRcvEIDS_R_JigB(strACK, strReason);
            }
            else // Don't Know Jig No -> output bit로 구분
            {
                if(m_PLCSocket.GetOutputA())
                {
                    SeqProcRcvEIDS_R_JigA(strACK, strReason);

                    m_GMESSocket.SetEIDS_R_Flag(0, true);
                }
                else if(m_PLCSocket.GetOutputB())
                {
                    SeqProcRcvEIDS_R_JigB(strACK, strReason);

                    m_GMESSocket.SetEIDS_R_Flag(1, true);
                }
            }
        }

        protected void SeqProcRcvEIDS_R_JigA(string strACK, string strReason)
        {
            if(strACK != "0") // Not Accepted
            {
                // Logging
                LogManager.WriteLog(LogType.Warning, "[Jig A] Output Error - Reason : " + strReason);

                // Set Output Error Flag & Display Error Msg
                m_PLCSocket.SetOutputErrorA(true);
                ucMISStatus.UpdateAlarm(strReason, false, eJigType.JigA.ToString());

                // Set Output Response ON(PLC) -- jigA
                if (m_PLCSocket.GetOutputA()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetOutputARes(true);
                }
            }
            else // Accepted
            {
                // Product Logging
                LogManager.ProductWriteLog(LogType.Information, "The product has been emission to Jig A. Model = [" + m_PLCSocket.GetPLCModel() + "], BarCode = [" + m_PLCSocket.GetBarcodeA() + "]", m_config.ProductLogPath);

                // Clear Errors
                m_PLCSocket.SetInputErrorA(false);
                m_PLCSocket.Set_A_Model_Error(false);
                m_PLCSocket.SetOutputErrorA(false);

                // Set Output Response
                if (m_PLCSocket.GetOutputA()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetOutputARes(true);
                }

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[GMES->PC] EIDS_R : Inspection Data Send Accepted", true, eJigType.JigA.ToString()); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경
            }
        }

        protected void SeqProcRcvEIDS_R_JigB(string strACK, string strReason)
        {
            if (strACK != "0") // Not Accepted
            {
                // Logging
                LogManager.WriteLog(LogType.Warning, "[Jig B] Output Error - Reason : " + strReason);

                // Set Output Error Flag & Display Error Msg
                m_PLCSocket.SetOutputErrorB(true);
                ucMISStatus.UpdateAlarm(strReason, false, eJigType.JigB.ToString());

                // Set Output Response ON(PLC) -- jigB
                if (m_PLCSocket.GetOutputB()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetOutputBRes(true);
                }
            }
            else // Accepted
            {
                // Product Logging
                LogManager.ProductWriteLog(LogType.Information, "The product has been emission to Jig B. Model = [" + m_PLCSocket.GetPLCModel() + "], BarCode = [" + m_PLCSocket.GetBarcodeB() + "]", m_config.ProductLogPath);

                // Clear Errors
                m_PLCSocket.SetInputErrorB(false);
                m_PLCSocket.Set_B_Model_Error(false);
                m_PLCSocket.SetOutputErrorB(false);

                // Set Output Response
                if (m_PLCSocket.GetOutputB()) // Req 신호가 살아 있을 경우에만 ON
                {
                    m_PLCSocket.SetOutputBRes(true);
                }

                // Set Ui to Normal
                ucMISStatus.UpdateAlarm("[GMES->PC] EIDS_R : Inspection Data Send Accepted", true, eJigType.JigB.ToString()); // sjyi 2024.05.21 진행 상황 표기를 위해, 변경
            }
        }

        // OFFLINE 모드
        // 연결상태 확인 후, 재연결 되면 INIT 모드로 변경
        protected void SeqProcOffline()
        {
            // 연결 상태 확인
            // 다시 연결되면 INIT 상태로 변경
            if (m_GMESSocket.IsConnected == true && m_PLCSocket.GetIsConnected() == true)
            {
                ResetMIS();
            }
        }

        // 화면 사이즈 변경시, 이벤트 함수
        // 최소화 할 경우, 시스템 트레이로 숨김
        // 다시 원래 사이즈로 돌아갈 경우, 태스크바에 화면 표시하고, 시스템 트레이 아이콘은 숨김.
        private void MainForm_Resize(object sender, EventArgs e)
        {
            if(this.WindowState == FormWindowState.Minimized)
            {
                notifyIcon1.Visible = true;
                this.Hide();
            }
            else if(this.WindowState == FormWindowState.Normal)
            {
                notifyIcon1.Visible = false;
                this.ShowInTaskbar = true;
            }
        }

        // 시스템 트레이에 있는 아이콘 더블 클릭 이벤트 함수
        // 더블 클릭 시, 화면 크기 복원
        private void notifyIcon1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            notifyIcon1.Visible = false;
            this.Show();
            this.WindowState = FormWindowState.Normal;
        }

        // 단축키 사용
        // F10 누를 때마다, 테스트 및 Maint용 버튼 보이기 <-> 숨기기 반복
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (Keys.F10 == keyData)
            {
                bool isVisble = !button_PLC_InputA.Visible;

                button_PLC_InputA.Visible   = isVisble;
                button_PLC_outputA.Visible  = isVisble;
                button_PLC_ErrorA.Visible   = isVisble;
                /*button_PLC_InputB.Visible   = isVisble;
                button_PLC_outputB.Visible  = isVisble;
                button_PLC_ErrorB.Visible   = isVisble;*/
                btn_Clear_GMES.Visible      = isVisble;
                btn_Clear_GMESA.Visible     = isVisble;
                /*btn_Clear_GMESB.Visible     = isVisble;
                buttonClearPLC.Visible = isVisble;*/
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        // 테스트용 버튼 ---------------------------------------------
        // Jig A의 Input Response 신호 Toggle
        private void button_PLC_InputA_Click(object sender, EventArgs e)
        {
            m_PLCSocket.SetInputARes(!m_PLCSocket.GetInputARes());
        }

        // Jig A의 Output Response 신호 Toggle
        private void button_PLC_outputA_Click(object sender, EventArgs e)
        {
            m_PLCSocket.SetOutputARes(!m_PLCSocket.GetOutputARes());
        }

        // Jig A의 Input Error 신호 Toggle
        private void button_PLC_ErrorA_Click(object sender, EventArgs e)
        {
            m_PLCSocket.SetInputErrorA(!m_PLCSocket.GetInputErrorA());

            // sjyi imsi
            //m_GMESSocket.m_clsJSON.ReadJsonFileAndRename("C:\\Project_Azure\\IWS_HAIRPIN_BEV3_LGMG2209\\MIS_2nd_Manual\\MIS\\bin\\recvEPDS.json");
            //m_GMESSocket.RcvProcEPDS();
            //SeqProcRcvEPDS(0);

            // sjyi imsi
            //m_PLCSocket.CloseSocket();
        }

        // Jig B의 Input Response 신호 Toggle
        private void button_PLC_InputB_Click(object sender, EventArgs e)
        {
            m_PLCSocket.SetInputBRes(!m_PLCSocket.GetInputBRes());
        }

        // Jig B의 Output Response 신호 Toggle
        private void button_PLC_outputB_Click(object sender, EventArgs e)
        {
            m_PLCSocket.SetOutputBRes(!m_PLCSocket.GetOutputBRes());
        }

        // Jig B의 Input Error 신호 Toggle
        private void button_PLC_ErrorB_Click(object sender, EventArgs e)
        {
            m_PLCSocket.SetInputErrorB(!m_PLCSocket.GetInputErrorB());
        }

        // Maint용 버튼 ---------------------------------------------
        // GMES Flag Clear
        private void btn_Clear_GMES_Click(object sender, EventArgs e)
        {
            m_GMESSocket.ClearAllFlag();
        }

        // A Jig의 GMES 송수신 Flag 및 정보 Clear
        private void btn_Clear_GMESA_Click(object sender, EventArgs e)
        {
            m_GMESSocket.ClearJigStatus(0);
        }

        // B Jig의 GMES 송수신 Flag 및 정보 Clear
        private void btn_Clear_GMESB_Click(object sender, EventArgs e)
        {
            m_GMESSocket.ClearJigStatus(1);
        }

        private void buttonClearPLC_Click(object sender, EventArgs e)
        {
            ResetMIS();
        }

        private void ResetMIS()
        {
            // Clear All Flag
            m_PLCSocket.ClearAllFlag();
            m_GMESSocket.ClearAllFlag();

            // Clear Alarm Status in UI
            ucMISStatus.UpdateAlarm("!!! Reset MIS !!!", true, eJigType.JigA.ToString());
            ucMISStatus.UpdateAlarm("!!! Reset MIS !!!", true, eJigType.JigB.ToString());

            // Set Main Seq to EPQ_INIT
            SetCurrentSeq((int)eEuipmentSeq.EQP_INIT);
            SetCurrentInitSeq(0);
        }

        // sjyi 2024.01.15 이전장비 검사결과 저장 함수
        private void SetPreInspectResult(int nJigNo)
        {
            // Get EPDS Data(GMES 데이터)
            EPDS eqpData = m_GMESSocket.GetProductData(nJigNo);

            // Check Direction, 회전 여부 검사
            bool bRotate = m_config.CheckRotationEquip(eqpData.strWeldEquipID);
            if (bRotate)
            {
                LogManager.WriteLog(LogType.Information, "Rotation SlotNo [" + eqpData.strWeldEquipID + "]");
            }

            // Parse Inspect Result
            ParseInspectResult(nJigNo, bRotate);      
        }

        // sjyi 2024.01.15 이전장비 검사결과 데이터(GMES 데이터)에서, 최종 검사 결과만 가져와서, PLC 데이터로 저장하는 함수
        private void ParseInspectResult(int nJigNo, bool bRotate)
        {

            // 변수 초기화
            m_lsPreInspResult.Clear();
            string strSlotLayer = "";   // Slot Layer String
            string strLayer = "";       // Layer No String
            string strResult = "";      // Pre Inspection Result String
            string strSubResult = "";   // 최종 결과 문자열
            int nFJIndex = 0;           // 최종 결과 문자열 Index
            int nLayerIndex = 0;        // Slot Layer 구분 Index

            List<PreInspectResult> lsPreInspResult = new List<PreInspectResult>();
            // Parse Inspect Result Data -->
            // Get Product Data(GMES Product Data)
            EPDS eqpData = m_GMESSocket.GetProductData(nJigNo);

            // Parsing
            bool bFind1stResult = false;
            bool b1stResultOK = true;
            //if (eqpData.strInspResult.Length > 0)
            //{
            //    bFind1stResult = true;
            //    if(eqpData.strInspResult == "NG")
            //    {
            //        b1stResultOK = false;
            //    }
            //}

            int nMaxSlot = 0;
            int nMaxLayer = 0;

            int nPassCnt = 0;
            int nReworkCnt = 0;
            int nOtherCnt = 0;

            for (int nWeldingInfoID = 0; nWeldingInfoID < eqpData.LsSubWedingInfo.Count(); nWeldingInfoID++)
            {
                PreInspectResult stPreInspResult = new PreInspectResult();

                // 문자열 가져오기
                strSlotLayer = eqpData.LsSubWedingInfo[nWeldingInfoID].strWeldInfoName;
                strResult = eqpData.LsSubWedingInfo[nWeldingInfoID].strWeldInfoResult;

                // Slot Layer No
                nLayerIndex = strSlotLayer.IndexOf('_');
                if(nLayerIndex > 0)
                {
                    stPreInspResult.nSlotNo = Convert.ToInt16(strSlotLayer.Substring(0, nLayerIndex));
                    strLayer = strSlotLayer.Substring(nLayerIndex + 1, 3);
                    switch (strLayer)
                    {
                        case "1-2":
                            stPreInspResult.nLayerNo = 1;
                            break;
                        case "3-4":
                            stPreInspResult.nLayerNo = 2;
                            break;
                        case "5-6":
                            stPreInspResult.nLayerNo = 3;
                            break;
                        case "7-8":
                            stPreInspResult.nLayerNo = 4;
                            break;
                    }
                }

                // Final Judge
                // 1 : OK, 2 : NG(Rework), 3 : ALARM, 4 : PASS
                int nAfterIndex = 0;
                string strAfterIndex;
                for(int i=0; i<10; i++)
                {
                    nAfterIndex++;
                    strAfterIndex = string.Format("D{0}", nAfterIndex);
                    nFJIndex = strResult.IndexOf(strAfterIndex);
                    if (nFJIndex >= 0)
                    {
                        //strSubResult = strResult.Substring(nFJIndex + 2, 2);
                        strSubResult = strResult.Substring(nFJIndex + 3, 2); // sjyi 2024.05.09 콤마(,)포함되어 인덱스 위치 변경

                        if (strSubResult == "OK") // PASS
                        {
                            stPreInspResult.nFinalJudge = 1;
                        }
                        else if (strSubResult == "NG") // Rework
                        {
                            stPreInspResult.nFinalJudge = 2;
                            
                        }
                        else // PASS
                        {
                            stPreInspResult.nFinalJudge = 4;
                            nOtherCnt++;
                        }
                    }
                    else
                    {
                        //stPreInspResult.nFinalJudge = 2; // 1 : OK, 2 : NG(Rework), 3 : ALARM, 4 : PASS
                        break;
                    }
                }

                // Count Up Final Result
                switch(stPreInspResult.nFinalJudge)
                {
                    case 1:
                        nPassCnt++;
                        break;
                    case 2:
                        nReworkCnt++;
                        break;
                    default:
                        nOtherCnt++;
                        break;
                }
                

                // Check Max Slot Layer
                if (stPreInspResult.nSlotNo > nMaxSlot)
                {
                    nMaxSlot = stPreInspResult.nSlotNo;
                }

                if(stPreInspResult.nLayerNo > nMaxLayer)
                {
                    nMaxLayer = stPreInspResult.nLayerNo;
                }

                // Add Result
                lsPreInspResult.Add(stPreInspResult);

                // Check 1st Eqp Judge
                if(bFind1stResult == false)
                {
                    if (stPreInspResult.nFinalJudge != 1)
                    {
                        b1stResultOK = false;
                    }
                }
            }

            // Sorting Data
            SortInspectResult(ref lsPreInspResult, nMaxSlot, nMaxLayer);

            // Make Binary Data(Include Rotation)
            int nSubIndex = 0;
            if(bRotate) // Rotate(180도 회전)
            {
                for (int i = 0; i < lsPreInspResult.Count; i++)
                {
                    nSubIndex = i + 48; // 0->48, 1->49, 2->50, ... , 47->95.

                    if(i >= 48)
                    {
                        nSubIndex = i - 48; // 48->0, 49->1, 50->2, ... , 95->47.
                    }

                    m_lsPreInspResult.Add(lsPreInspResult[nSubIndex]);
                }
            }
            else // 회전안함 그대로 저장
            {
                for(int i=0; i<lsPreInspResult.Count; i++)
                {
                    m_lsPreInspResult.Add(lsPreInspResult[i]);
                }
            }

            // Convert Inspect Result(int) to Bytes Data
            List<byte> writePlcList = new List<byte>();
            for (int i = 0; i<m_lsPreInspResult.Count; i++)
            {
                writePlcList.AddRange(BitConverter.GetBytes(m_lsPreInspResult[i].nFinalJudge));
            }


            // 용접 후 검사 결과 Write
            int nStartAdress = 0;
            
            switch(nJigNo)
            {
                case 0: // JigA
                    nStartAdress = Convert.ToInt32(m_config.PLC_ReadAftInspData_A);
                    break;
                case 1: // JigB
                    nStartAdress = Convert.ToInt32(m_config.PLC_ReadAftInspData_B);
                    break;
            }

            int nWordSize = writePlcList.Count / 2;
            LogManager.WriteLog(LogType.Information, 
                "[PC->PLC] Write PreInspectResult(" + nJigNo.ToString() + 
                "), Address : " + nStartAdress.ToString() + 
                ", Size : " + nWordSize.ToString() +
                ", OK(1) : " + nPassCnt.ToString() +
                ", Rework(2) : " + nReworkCnt.ToString());
            m_PLCSocket.WriteToPLC(nStartAdress, writePlcList.Count / 2, writePlcList.ToArray());

			// sjyi 2024.05.21 진행 상황 표기를 위해, 추가
            // Set Ui to Normal & Report Repair Point
            string strReport = "";
            strReport = string.Format("TotalPoint : {0}, OK : {1}, Rework : {2}", nWordSize, nPassCnt, nReworkCnt);
            ucMISStatus.UpdateAlarm(strReport, true, eJigType.JigA.ToString());

            // sjyi 2024.05.09, 1차 장비 용접 후 검사 전체 결과 Write
            if(b1stResultOK)
            {
                m_PLCSocket.Set1stResult(1);
            }
            else
            {
                m_PLCSocket.Set1stResult(2);
            }

        }

        private void SortInspectResult(ref List<PreInspectResult> lsInspectResult, int nMaxSlot, int nMaxLayer)
        {
            List<PreInspectResult> lsInspResultTemp = new List<PreInspectResult>();


            for (int nSlot = 0; nSlot < nMaxSlot; nSlot++)
            {
                for(int nLayer = 0; nLayer < nMaxLayer; nLayer++)
                {
                    PreInspectResult stPreInspResult = new PreInspectResult();

                    stPreInspResult.nFinalJudge = 2; // Rework
                    stPreInspResult.nSlotNo = nSlot+1;
                    stPreInspResult.nLayerNo = nLayer+1;

                    for (int i=0; i< lsInspectResult.Count(); i++)
                    {
                        if(lsInspectResult[i].nSlotNo == nSlot + 1 && lsInspectResult[i].nLayerNo == nLayer+1)
                        {
                            stPreInspResult.nFinalJudge = lsInspectResult[i].nFinalJudge;
                            break;
                        }
                    }
                    lsInspResultTemp.Add(stPreInspResult);
                }
            }

            lsInspectResult = lsInspResultTemp;
        }

        private void CheckLinkPLC()
        {
            bool bLinkPLC = m_PLCSocket.GetLink();
            if (bLinkPLC != m_bLinkPLC) // 신호 변경 되면
            {
                m_bLinkPLC = bLinkPLC; // 현재 신호값 변경

                //// End 시간 획득
                //m_LinkEnd = DateTime.Now;

                //// Check Time Out
                //TimeSpan ts = m_LinkEnd - m_LinkStart;

                //// imsi
                //string strLog = string.Format("LinkTime {0} msec", ts.TotalMilliseconds);
                //LogManager.WriteLog(LogType.Information, strLog);

                m_LinkStart = DateTime.Now; // Timeout 시작 시간 변경
            }
            else // 신호 변경 없으면,
            {
                // End 시간 획득
                m_LinkEnd = DateTime.Now;
                
                // Check Time Out
                TimeSpan ts = m_LinkEnd - m_LinkStart;

                if (ts.TotalSeconds > 5)// 5초 이상 지연 -> Timeout 재접속
                {
                    LogManager.WriteLog(LogType.Error, "!!! LinkTest Timeout !!!");
                    m_PLCSocket.CloseSocket();

                    m_LinkStart = DateTime.Now; // Timeout 시작 시간 변경
                }

            }
        }
    }
}
