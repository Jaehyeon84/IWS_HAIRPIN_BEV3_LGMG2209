using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using HubisCommonControl;

namespace MIS
{
    static class Program
    {
        /// <summary>
        /// 해당 응용 프로그램의 주 진입점입니다.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // sjyi 2024.05.24 프로세스 이름으로 할 경우, 오류 발생하여, Mutex로 변경
            //System.Diagnostics.Process[] processes = null;
            //string strCurrentProgram = System.Diagnostics.Process.GetCurrentProcess().ProcessName.ToUpper();
            //processes = System.Diagnostics.Process.GetProcessesByName(strCurrentProgram);
            //if (processes.Length > 1)
            //{
            //    MessageBox.Show("The Program is running redundantly.");
            //    LogManager.WriteLog(LogType.Error, "The Program is running redundantly.");
            //    return;
            //}

            bool flagMutex;

            System.Threading.Mutex m_hMutex = new System.Threading.Mutex(true, "MISManual", out flagMutex);
            if (flagMutex == false)
            {
                // 여러개 실행시켰을때 띄울 메시지
                MessageBox.Show("The Program is running redundantly.");
                LogManager.WriteLog(LogType.Error, "The Program is running redundantly.");
                return;
            }

            // 프로그램 실행
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());

            // 뮤텍스 해제
            m_hMutex.ReleaseMutex();

        }
    }
}
