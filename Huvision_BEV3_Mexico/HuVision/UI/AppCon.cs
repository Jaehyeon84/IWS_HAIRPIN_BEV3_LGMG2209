using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace HuVision.UI
{
    public class AppCon : System.Windows.Forms.Panel
    {
         /// <summary>
        /// Track if the application has been created
        /// </summary>
        bool created = false;

        /// <summary>
        /// Handle to the application Window
        /// </summary>
        IntPtr appWin;

        /// <summary>
        /// The name of the exe to launch
        /// </summary>
        private string exeName = "";


        /// <summary>
        /// The argumens of the exe to launch
        /// </summary>
        private string exeArgs = "";

        private string exeSearch = "";

        /// <summary>
        /// Get/Set if we draw the tick marks
        /// </summary>
        [
        Category("Data"),
        Description("Name of the executable to launch"),
        DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)
        ]
        public string ExeName
        {
            get
            {
                return exeName;
            }
            set
            {
                exeName = value;
            }
        }

        [
Category("Data"),
Description("Arguments of the executable to launch"),
DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)
]
        public string ExeArgs
        {
            get
            {
                return exeArgs;
            }
            set
            {
                exeArgs = value;
            }
        }

        [
Category("Data"),
Description("Search of the executable to launch"),
DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)
]
        public string ExeSearch
        {
            get
            {
                return exeSearch;
            }
            set
            {
                exeSearch = value;
            }
        }


        [DllImport("user32.dll", EntryPoint = "GetWindowThreadProcessId", SetLastError = true,
             CharSet = CharSet.Unicode, ExactSpelling = true,
             CallingConvention = CallingConvention.StdCall)]
        private static extern long GetWindowThreadProcessId(long hWnd, long lpdwProcessId);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern long SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll", EntryPoint = "GetWindowLongA", SetLastError = true)]
        private static extern long GetWindowLong(IntPtr hwnd, int nIndex);

        [DllImport("user32.dll", EntryPoint = "SetWindowLongA", SetLastError = true)]
        private static extern long SetWindowLong(IntPtr hwnd, int nIndex, long dwNewLong);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern long SetWindowPos(IntPtr hwnd, long hWndInsertAfter, long x, long y, long cx, long cy, long wFlags);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool MoveWindow(IntPtr hwnd, int x, int y, int cx, int cy, bool repaint);

        [DllImport("user32.dll", EntryPoint = "PostMessageA", SetLastError = true)]
        private static extern bool PostMessage(IntPtr hwnd, uint Msg, long wParam, long lParam);

        private const int SWP_NOOWNERZORDER = 0x200;
        private const int SWP_NOREDRAW = 0x8;
        private const int SWP_NOZORDER = 0x4;
        private const int SWP_SHOWWINDOW = 0x0040;
        private const int WS_EX_MDICHILD = 0x40;
        private const int SWP_FRAMECHANGED = 0x20;
        private const int SWP_NOACTIVATE = 0x10;
        private const int SWP_ASYNCWINDOWPOS = 0x4000;
        private const int SWP_NOMOVE = 0x2;
        private const int SWP_NOSIZE = 0x1;
        private const int GWL_STYLE = (-16);
        private const int WS_VISIBLE = 0x10000000;
        private const int WM_CLOSE = 0x10;
        private const int WS_CHILD = 0x40000000;

        /// <summary>
        /// Force redraw of control when size changes
        /// </summary>
        /// <param name="e">Not used</param>
        protected override void OnSizeChanged(EventArgs e)
        {
            this.Invalidate();
            base.OnSizeChanged(e);
        }


        ///// <summary>
        ///// Creeate control when visibility changes
        ///// </summary>
        ///// <param name="e">Not used</param>
        //protected override void OnVisibleChanged(EventArgs e)
        //{
        //    /*
        //    if (string.IsNullOrEmpty(this.exeName))
        //        return;

        //    // If control needs to be initialized/created
        //    if (created == false)
        //    {

        //        // Start the remote application
        //        try
        //        {
        //            // Initialize handle value to invalid
        //            appWin = IntPtr.Zero;

        //            Process p = null;

        //            // Start the process
        //            p = System.Diagnostics.Process.Start(this.exeName, this.exeArgs);

        //            // Wait for process to be created and enter idle condition
        //            p.WaitForInputIdle();

        //            // Get the main handle
        //            appWin = p.MainWindowHandle;

        //            // Put it into this form
        //            SetParent(appWin, this.Handle);

        //            // Remove border and whatnot
        //            SetWindowLong(appWin, GWL_STYLE, WS_VISIBLE);

        //            // Move the window to overlay it on this window
        //            MoveWindow(appWin, 0, 0, this.Width, this.Height, true);

        //            // Mark that control is created
        //            created = true;
        //        }
        //        catch (Exception ex)
        //        {
        //            created = false;

        //            MessageBox.Show(this, ex.Message, "Error");
        //        }			


        //    }
        //    */
        //    base.OnVisibleChanged (e);
        //}


        /// <summary>
        /// 
        /// </summary>
        /// <param name="e"></param>
        protected override void OnHandleDestroyed(EventArgs e)
        {
            // Stop the application
            if (appWin != IntPtr.Zero)
            {

                // Post a colse message
                PostMessage(appWin, WM_CLOSE, 0, 0);

                // Delay for it to get the message
                System.Threading.Thread.Sleep(1000);

                // Clear internal handle
                appWin = IntPtr.Zero;

            }

            base.OnHandleDestroyed(e);
        }


        /// <summary>
        /// Update display of the executable
        /// </summary>
        /// <param name="e">Not used</param>
        protected override void OnResize(EventArgs e)
        {
            if (this.appWin != IntPtr.Zero)
            {
                MoveWindow(appWin, 0, 0, this.Width, this.Height, true);
            }
            base.OnResize(e);
        }

        public bool Execute(string exeName, string exeArgs)
        {
            this.exeName = exeName;
            this.exeArgs = exeArgs;

            return Execute();
        }


        public bool Execute(object obj)
        {
            if (string.IsNullOrEmpty(this.exeName))
                return false;

            if (created)
                return false;

            // If control needs to be initialized/created
            // Start the remote application
            try
            {
                // Initialize handle value to invalid
                appWin = IntPtr.Zero;

                Process p = null;

                // Start the process
                // [ORIGINAL] => [ISPARK]
                // [ORIGINAL] -->
                //p = System.Diagnostics.Process.Start(this.exeName, this.exeArgs);
                // --> [ORIGINAL]
                // [ISPARK] -->
                ProcessStartInfo psi = new ProcessStartInfo(this.exeName, this.exeArgs);
                psi.WorkingDirectory = this.exeName.Substring(0, this.exeName.LastIndexOf('\\'));
                p = new Process();
                p.StartInfo = psi;
                p.Start();
                // --> [ISPARK]


                // Wait for process to be created and enter idle condition
                p.WaitForInputIdle();

                // Get the main handle
                appWin = p.MainWindowHandle;

                // Put it into this form
                SetParent(appWin, this.Handle);

                // Remove border and whatnot
                SetWindowLong(appWin, GWL_STYLE, WS_VISIBLE);

                // Move the window to overlay it on this window
                MoveWindow(appWin, 0, 0, this.Width, this.Height, true);

                // Mark that control is created
                created = true;
            }
            catch (Exception ex)
            {
                created = false;

                MessageBox.Show(this, ex.Message, "Error");
            }

            return created;
        }

        System.Threading.AutoResetEvent pEvent = new System.Threading.AutoResetEvent(false);

        Process p;

        public bool Execute()
        {
            try
            {
                if (string.IsNullOrEmpty(this.exeName))
                    return false;

                if (created)
                    return false;

                appWin = IntPtr.Zero;

                ProcessStartInfo psi = new ProcessStartInfo(this.exeName, this.exeArgs);
                psi.WorkingDirectory = this.exeName.Substring(0, this.exeName.LastIndexOf('\\'));
                p = new Process();
                p.StartInfo = psi;
                p.Start();
                p.PriorityBoostEnabled = true;
                p.WaitForInputIdle();

                System.Threading.Thread thd = new System.Threading.Thread(CheckProcessMainWindowsHandle);
                thd.Start();

                pEvent.Reset();
                pEvent.WaitOne(5000);

                thd.Abort();

                if (appWin.ToInt64() > 0)
                {
                    SetParent(appWin, this.Handle);

                    SetWindowLong(appWin, GWL_STYLE, WS_VISIBLE);

                    MoveWindow(appWin, 0, 0, this.Width, this.Height, true);
                }
                created = true;
            }
            catch (Exception ex)
            {
                created = false;

                MessageBox.Show(this, ex.Message, "Error");
            }
            return created;
        }

        public bool SearchExcute(int waitTime)
        {
            try
            {
                if (string.IsNullOrEmpty(this.exeSearch))
                    return false;

                if (created)
                    return false;

                appWin = IntPtr.Zero;

                //ProcessStartInfo psi = new ProcessStartInfo(this.exeName, this.exeArgs);
                //psi.WorkingDirectory = this.exeName.Substring(0, this.exeName.LastIndexOf('\\'));
                //p = new Process();
                //p.StartInfo = psi;
                //p.Start();
                //p.PriorityBoostEnabled = true;
                //p.WaitForInputIdle();

                System.Threading.Thread thd = new System.Threading.Thread(SearchCheckProcessMainWindowsHandle);
                thd.Start();

                pEvent.Reset();
                pEvent.WaitOne(waitTime);

                thd.Abort();

                //int tmpWaitTime = 0;
                //while (true)
                //{
                //    Process[] processArr = Process.GetProcessesByName(exeSearch);
                //    while (processArr.Length < 1)
                //    {
                //        if (tmpWaitTime >= waitTime)
                //            return false;

                //        processArr = Process.GetProcessesByName(exeSearch);
                //        if (processArr.Length > 1)
                //            break;

                //        System.Threading.Thread.Sleep(100);
                //        tmpWaitTime += 100;
                //    }

                //    appWin = processArr[0].MainWindowHandle;

                //    if (appWin.ToInt64() > 0 )
                //        break;
                //    else
                //        System.Threading.Thread.Sleep(100);
                //}

                if (appWin.ToInt64() > 0)
                {
                    SetParent(appWin, this.Handle);

                    SetWindowLong(appWin, GWL_STYLE, WS_VISIBLE);

                    MoveWindow(appWin, 0, 0, this.Width, this.Height, true);
                }
                created = true;
            }
            catch (Exception ex)
            {
                created = false;

                MessageBox.Show(this, ex.Message, "Error");
            }
            return created;
        }

        private void SearchCheckProcessMainWindowsHandle(object obj)
        {
            while (true)
            {
                Process[] processArr = Process.GetProcessesByName(exeSearch);
                while(processArr.Length < 1)
                    processArr = Process.GetProcessesByName(exeSearch);

                appWin = processArr[0].MainWindowHandle;

                if (appWin.ToInt64() > 0)
                    break;
                else
                    System.Threading.Thread.Sleep(100);
            }
            pEvent.Set();
        }

        private void CheckProcessMainWindowsHandle(object obj)
        {
            while (true)
            {
                appWin = p.MainWindowHandle;

                if (appWin.ToInt64() > 0)
                    break;
                else
                    System.Threading.Thread.Sleep(100);
            }
            pEvent.Set();
        }

    }


}
