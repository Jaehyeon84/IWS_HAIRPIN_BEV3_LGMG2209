using HuVision.Model;
using HuVision.Sock;
using HuVision.UI;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HuVision
{
    public partial class MainForm : System.Windows.Forms.Form
    {
        private static Image toggleOn = Properties.Resources.Toggle_on;
        private static Image toggleOff = Properties.Resources.Toggle_off;        

        private UcInspection ucInspection;

        private SockManagement sockManagement;

        private bool isWorker = true;
        private Thread monitoringThread;

        private IniManagement iniManagement;
        //private DBManagement dbManagement = new DBManagement();

        public MainForm()
        {
            InitializeComponent();

            iniManagement = IniManagement.Instance();

            if (!iniManagement.OpenIniModel())
            {
                LogManager.WriteLog(LogType.Error, "Config Read Fail");
                MessageBox.Show("Config Read Fail");
                Application.ExitThread();
                Environment.Exit(0);
            }

            //dbManagement.Init();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            lbl_Name.Text = iniManagement.iniModel.Process_Name;
            cb_Manual.Checked = iniManagement.iniModel.Process_Manual;

            ucInspection = new UcInspection();
            ucInspection.Dock = DockStyle.Fill;
            pnl_Main.Controls.Add(ucInspection);

            sockManagement = new SockManagement();
            if (!iniManagement.iniModel.Process_Manual)
            {
                //AddAppControl(iniManagement.iniModel.Process_Path);
                sockManagement.LogAppended += sockManagement_LogAppended;
                sockManagement.AfterInspected += sockManagement_AfterInspected;
                sockManagement.BeforeInserted += sockManagement_BeforeInserted;
                sockManagement.Open();

                monitoringThread = new Thread(new ThreadStart(Worker));
                monitoringThread.Start();
            }

            this.BringToFront();
            this.Focus();

            this.WindowState = FormWindowState.Minimized;
        }

        private void Worker()
        {
            while(isWorker)
            {
                try
                {
                    if (sockManagement == null)
                        continue;

                    this.Invoke(new MethodInvoker(delegate()
                    {
                        // In
                        pb_InLink.Image = sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.LINK] == 1 ? toggleOn : toggleOff;
                        pb_InspReq.Image = sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.INSP] == 1 ? toggleOn : toggleOff;

                        lbl_Model.Text = sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.PLC_MODEL].ToString();
                        lbl_Rcp.Text = sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.RCP].ToString();
                        lbl_LotID.Text = sockManagement.plcModel.lotIdBuild.ToString();

                        // Out
                        pb_OutLink.Image = sockManagement.plcModel.outputSignal[(int)OUTPUT_FRAME.LINK] == 1 ? toggleOn : toggleOff;
                        pb_InspRes.Image = sockManagement.plcModel.outputSignal[(int)OUTPUT_FRAME.INSP] == 1 ? toggleOn : toggleOff;

                        //lbl_Result.Text = sockManagement.plcModel.outputSignal[(int)OUTPUT_FRAME.AFT_RESULT].ToString();
                    }));

                    //ucInspection.SetRcp(sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.RCP]);
                    ucInspection.SetModel(sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.PLC_MODEL]);
                }
                catch { }
                finally
                {
                    Thread.Sleep(300);
                }
            }
        }

        void sockManagement_BeforeInserted(object sender, EventArgs e)
        {
            //dbManagement.JugementInsert(sockManagement.plcModel.lotIdBuild.ToString());
        }

        void sockManagement_AfterInspected(string path, string lotID)
        {
            AppendLog("After Insp Path: " + path);            
            AppendLog("After Insp LotID: " + lotID);

            Thread afterInspectionThread = new Thread(() => AfterInspectionWorker(path, lotID));
            afterInspectionThread.Start();
        }

        private void AfterInspectionWorker(object path, object lotID)
        {
            //if (this.InvokeRequired)
            //{
            //    this.Invoke(new MethodInvoker(delegate()
            //    {
            //        AftInspectionProcessing(path.ToString(), lotID.ToString(), sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.RCP]);
            //    }));
            //}
            //else
            //{
            //    AftInspectionProcessing(path.ToString(), lotID.ToString(), sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.RCP]);
            //}

            AftInspectionProcessing(path.ToString(), lotID.ToString(), sockManagement.plcModel.inputSignal[(int)INPUT_FRAME.RCP]);
        }

        private void AftInspectionProcessing(string binPath, string lotID, int modelNo, bool isManual = false)
        {
            ucInspection.AppendLog("Start");

            //string imgPath = string.Empty;

            //// 0: D:
            //// 1: InsptAft
            //// 2: ModelName
            //// 3: year, 4: month, 5: day
            //// 6: HHmmss_LOTID.bin
            //if (isManual)
            //{                
            //    // path = D:\Inspt_aft\Year\Month\Day (Image Path)
            //    binPath = binPath.Split('.')[0];

            //    // 타겟 이미지 경로
            //    string[] tmpTokken = binPath.Split('\\');
            //    string[] tmpFileName = tmpTokken[6].Split('.');

            //    // 원본 이미지 경로
            //    string sourceImgPath = @"D:\Inspt_aft\Year\Month\Day\" + binPath + ".png";

            //    imgPath = string.Format(@"{0}\{1}\{2}\{3}\{4}\{5}\{6}.png",
            //        tmpTokken[0], tmpTokken[1], tmpTokken[2], tmpTokken[3], tmpTokken[4], tmpTokken[5], tmpFileName[0]);

            //    File.Copy(sourceImgPath, imgPath);
            //    Thread.Sleep(500);
            //}

            //string dataPath = binPath + ".bin";

            // D:\InsptAft\ModelName\Year\Month\Day\HHmmss_LOTID.png
            // 시간 추출
            //string[] tokken = imgPath.Split('\\');          
            //DateTime nowTime = DateTime.ParseExact(tokken[3] + tokken[4] + tokken[5] + tokken[tokken.Length - 1].Remove(6), "yyyyMMddHHmmss", null);

			// sjyi 2023.07.20  2차 용접기 재용접 포인트만 검사하기 위한 내용
            if(iniManagement.iniModel.Process_ReadRework)
            {
                byte[] ReadReultData = sockManagement.GetPLCResultData();
                ucInspection.MakeDataFile(lotID, iniManagement.iniModel.Process_MakePreData, ReadReultData);

                sockManagement.OnAppendLog(string.Format("Write Previous Data - LOTID : {0}", lotID));
            }

            if (!ucInspection.StartWeldAfterInspect(binPath, modelNo))
                return;

            DateTime nowTime = DateTime.Now;
            bool result = true;
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    result = ucInspection.Processing(binPath, lotID, modelNo, nowTime);
                }));
            }
            else
            {
                result = ucInspection.Processing(binPath, lotID, modelNo, nowTime);
            }

            if (!result)
                return;

            if (cb_Manual.Checked || iniManagement.iniModel.Process_Manual)  // 매뉴얼 모드 또는 테스트 모드 일때는 PLC 처리를 하지 않음
                return;

            ComparisonAndAnalysis();
            //if(ComparisonAndAnalysis())
            //{
            //    sockManagement.totalResult = ucInspection.totalResult;
                
            //    //Thread.Sleep(1000);

            //    // 후검사 데이터 DB 저장
            //    //dbManagement.AftInspInsert(nowTime.ToString("yyyy-MM-dd HH:mm:ss"), lotID,
            //    //    ucInspection.okCount, ucInspection.reworkCount, ucInspection.failCount, ucInspection.totalResult, ucInspection.saveCsvPath, ucInspection.saveImgPath);
            //}

            sockManagement.totalResult = ucInspection.totalResult;
            sockManagement.isWriteAfterResult = true;

            //sockManagement.ResultWriteToPLC(iniManagement.iniModel.PLC_WriteAfterResult, isAlarm, ucInspection.totalResult);
            //ucInspection.AppendLog("End");            
        }

        private bool ComparisonAndAnalysis()
        {
            byte[] writeResultData = ucInspection.GetWriteResultData();//sockManagement.befInspModelList);
            if (writeResultData == null)
            {
                AppendLog("GetWriteResultData Failed");
                return false;
            }

            sockManagement.SetWritePlcList(writeResultData);

            //if (!sockManagement.WriteAfterInspResult(iniManagement.iniModel.PLC_WriteReWorkData, writeResultData))
            //    return true;

            //if (!sockManagement.WriteMxConponent(iniManagement.iniModel.PLC_WriteReWorkData, writeResultData, "PLC Write Result Data", "PLC Write Fail - Result"))
            //    return true;

            return true;
        }        

        void sockManagement_LogAppended(string msg)
        {
            AppendLog(msg);
        }

        private void AppendLog(string msg)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    ucInspection.AppendLog(msg);
                }));
            }
            else
            {
                ucInspection.AppendLog(msg);
            }
        }

        private void ChangeConnectStatus(PictureBox pb, Image toggle)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new MethodInvoker(delegate()
                {
                    pb.Image = toggle;
                }));
            }
            else
            {
                pb.Image = toggle;
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            AppendLog("Program Exit");
            //new IniManagement().SaveIni(iniModel, Application.StartupPath + "\\WeldAfterInspect.ini");

            //dbManagement.Close();

            if (sockManagement != null)
                sockManagement.Close();

            isWorker = false;
            try
            {
                if (monitoringThread != null)
                {
                    monitoringThread.Abort();
                    monitoringThread = null;
                }
            }
            catch { }

            Application.ExitThread();
            Environment.Exit(0);
        }

        private void btn_Load_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDlg = new OpenFileDialog();
            openFileDlg.DefaultExt = "BIN";
            openFileDlg.Filter = "BIN Files (*.bin)|*.bin";
            if (openFileDlg.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    tbx_ManualPath.Clear();
                    tbx_ManualPath.Text = openFileDlg.FileName.Split('.')[0];

                    btn_ManualSave_Click(null, null);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private void btn_ManualSave_Click(object sender, EventArgs e)
        {
            Thread afterInspectionThread = new Thread(() => AfterInspectionWorker(tbx_ManualPath.Text, "MANUAL"));
            afterInspectionThread.Start();

            //AftInspectionProcessing(tbx_ManualPath.Text, "MANUAL", int.Parse(tbx_Model.Text), true);                
        }


        private void cb_Manual_CheckedChanged(object sender, EventArgs e)
        {
            iniManagement.iniModel.Process_Manual = cb_Manual.Checked;
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if(Keys.F10 == keyData)
            {
                bool isVisible = !cb_Manual.Visible;

                cb_Manual.Visible = isVisible;
                lbl_Path.Visible = isVisible;
                tbx_ManualPath.Visible = isVisible;
                btn_ManualSave.Visible = isVisible;
                btn_Load.Visible = isVisible;
                tbx_Model.Visible = isVisible;
            }

            return base.ProcessCmdKey(ref msg, keyData);
        }

        private void pb_Click(object sender, EventArgs e)
        {
            //PictureBox pb = sender as PictureBox;
            //pb.Image = pb.Image == toggleOn ? toggleOff : toggleOn;

            //short start = pb_InspReq.Image == toggleOn ? (short)1 : (short)0;
            //short end = pb_EndReq.Image == toggleOn ? (short)1 : (short)0;
            //string mode = lbl_Rcp.Text;
            //if (!(mode == "1" || mode == "2"))
            //    mode = "1";

            //sockManagement.ManualProcess(start, end, mode);
        }

        //private void Killps(string processName)
        //{
        //    Process[] process = Process.GetProcessesByName(processName);
        //    Process currentProcess = Process.GetCurrentProcess();
        //    foreach (Process p in process)
        //    {
        //        if (p.Id != currentProcess.Id)
        //            p.Kill();
        //    }
        //}

        private void pb_Config_Click(object sender, EventArgs e)
        {
            FrmConfig frmConfig = new FrmConfig();
            frmConfig.SaveClicked += offsetForm_SaveClicked;
            frmConfig.ShowDialog();
        }

        void offsetForm_SaveClicked(object sender, EventArgs e)
        {
            ucInspection.GridViewRefreshData();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //sockManagement.SaveLastPosition();
        }

		// sjyi 2023.07.20  2차 용접기 후검사를 위한 파일 만드는 함수(테스트용)
        private void WriteTest_Click(object sender, EventArgs e)
        {

            string strLOTID = sockManagement.plcModel.lotIdBuild.ToString();
            if(strLOTID.Length == 0)
            {
                strLOTID = "Bin";
            }

            byte[] ReadReultData = sockManagement.GetPLCResultData();
            ucInspection.MakeDataFile(strLOTID, true, ReadReultData);
        }
    }
}
