using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Collections;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace HuVision
{
    public class FrameLogManager
	{
		#region 멤버
		private const string LogFolder = "Frame";
        //private static string m_strProductName = "m-Bizmaker2";
		/// <summary>
		/// 로그파일이 저장될 폴더
		/// 예)C:\m-Bizmaker2\Bos\log
		/// </summary>
		private static string m_strLogFolder;
		#endregion

		#region 속성
		/// <summary>
		/// 로그폴더
		/// </summary>
		private static string BaseFolder 
		{
            get
            {
                if (m_strLogFolder == null)
                {
                    m_strLogFolder = Application.StartupPath.TrimEnd(new char[] { '\\' });
                    m_strLogFolder += '\\' + LogFolder;
                    m_strLogFolder += '\\' + DateTime.Now.ToString("yyyy");
                    m_strLogFolder += '\\' + DateTime.Now.ToString("MM");

                    if(!Directory.Exists(m_strLogFolder))
                        Directory.CreateDirectory(m_strLogFolder);
                }

                return m_strLogFolder;
            }

            //get
            //{
            //    if (null == m_strLogFolder)
            //    {
            //        string strLogFolder = _Registry.ReadString ("Module", "ISDir");
            //        if (false != string.IsNullOrEmpty (strLogFolder))
            //        {
            //            strLogFolder = @"C:\m-Bizmaker2";
            //        }

            //        strLogFolder = Path.Combine (strLogFolder.TrimEnd (new char[] {'\\'}), "BOS");
            //        strLogFolder = Path.Combine (strLogFolder, LogFolder);

            //        try
            //        {
            //            if (false == Directory.Exists (strLogFolder))
            //            {
            //                Directory.CreateDirectory (strLogFolder);
            //            }
            //        }
            //        catch (System.Exception e)
            //        {
            //            /*예상예외:
            //             * IOException
            //             * UnauthorizedAccessException
            //             * DirectoryNotFoundException
            //             --------------------------*/
            //            /*예외발생하면 실행폴도로 설정한다. */
            //            LogManager.WriteLog (e);
            //            strLogFolder = Application.StartupPath.TrimEnd (new char[] { '\\'});
            //        }

            //        m_strLogFolder = strLogFolder;
            //    }
            //    return m_strLogFolder;
            //}

		}
		/// <summary>
		/// 사용할 로그파일
		/// </summary>
		public static string LogFilePath
		{
			get
			{
				return	string.Format ("{0}\\{1:yyyy-MM-dd}.log", BaseFolder, DateTime.Now);
			}
		}

        ///// <summary>
        ///// 제품명 설정.
        ///// </summary>
        ///// <param name="strProductName"></param>
        //public static void SetProductName (string strProductName)
        //{
        //    m_strProductName = strProductName;
        //}
		#endregion

		#region 로그작업
		/// <summary>
		/// 로그를 파일에 기록.
		/// </summary>
		/// <param name="logType"></param>
		/// <param name="logContents"></param>
		public static void WriteLog (LogType logType, string logContents)
		{
			if (false == string.IsNullOrEmpty (logContents))
			{
				string strType = "Info";
				switch (logType)
				{
					case LogType.Warning:
						strType = "Warning";
						break;
					case LogType.Error:
						strType = "Error";
						break;
				}
                WriteLogFile(string.Format("[{0:yyyy-MM-dd HH:mm:ss.fff}]\t[{1}]\t{2}", DateTime.Now, strType, logContents));
                //WriteLogFile(string.Format("[{0:HH:mm:ss}] [{1}] {2}", DateTime.UtcNow, strType, logContents));
			}
		}

        public static void WriteLog(LogType logType, Exception e)
        {
            string type = "Error";
            System.Reflection.MethodBase site = e.TargetSite;
            string methodName = site == null ? "Empty" : site.Name;

            string logMsg = string.Format("{0} {1}", methodName, e.Message);
            WriteLogFile(string.Format("[{0:yyyy-MM-dd HH:mm:ss.fff}]\t[{1}]\t{2}", DateTime.Now, type, logMsg));
        }


        ///// <summary>
        ///// 오류로그 기록
        ///// </summary>
        ///// <param name="exception"></param>
        //public static void WriteLog(System.Exception exception)
        //{

        //    //if (exception is _PQException)
        //    //{
        //    //    _PQException pException = exception as _PQException;
        //    //    if (pException.IsEmpty)
        //    //        return;
        //    //}

        //    //StringBuilder sbLog = new StringBuilder ();
        //    //sbLog.AppendFormat ("{0:yyyy-MM-dd HH.mm.ss}\t오류\r\n", DateTime.Now);
        //    //GetLogContent (ref sbLog, exception);
        //    //WriteLogFile (sbLog.ToString ());
        //}

		/// <summary>
		/// 기능: Exception에 데이터를 입력한다. (중복키를 제거하기 위해 사용.) 
		/// </summary>
		/// <param name="exception"></param>
        public static void SetAddDataToException(System.Exception exception, string key, object value)
		{
			if (null != exception && false == string.IsNullOrEmpty (key))
			{
				if (false == exception.Data.Contains (key))
					exception.Data.Add (key, value);
			}
		}

		/// <summary>
		/// 로그파일을 삭제한다. 
		/// </summary>
		public static void DeleteLogFiles ()
		{
			DirectoryInfo logdirectory = new DirectoryInfo (BaseFolder);

			FileInfo[] files = logdirectory.GetFiles ("*????-??-??.log");

			if (null == files)
				return;

			DateTime dateTemp = DateTime.Now;
			DateTime nowDate = DateTime.Now;

			foreach (FileInfo file in files)
			{
				if (file.Exists)
				{
					if (GetFileDate (ref dateTemp, file.Name))
					{
						TimeSpan span = nowDate - dateTemp;
						if (span.TotalDays > 10)
						{
							try
							{
								file.Delete ();
							}
							catch
							{
							}
						}
					}
				}
			}
		}

		static Regex m_regKeywords = null;
		/// <summary>
		/// 로그파일형식을 체크하기 위한 정규식
		/// </summary>
		private static Regex RegLogFileKeywords 
		{
			get 
			{
				if (null == m_regKeywords)
					m_regKeywords = new Regex (@"\w+[0-9]{4}-[0-1][0-9]-[0-3][0-9]\.log$", RegexOptions.IgnoreCase | RegexOptions.Compiled);
				return m_regKeywords;				
			}
		}

		/// <summary>
		/// 로그파일에서 시간부분을 분리한다. 
		/// </summary>
		/// <param name="dateTemp">로그파일에서 분리된 시간[날짜]</param>
		/// <param name="strFile">제품명yyyy-mm-dd.log 형태의 파일명...</param>
		/// <returns>로그파일이 맞는지 여부.</returns>
		private static bool GetFileDate (ref DateTime dateTemp, string strFile)
		{
			if (string.IsNullOrEmpty (strFile))
				return false;

			int nLength = strFile.Length;
			if (14 > nLength)
				return false;

			Regex regKeywords = RegLogFileKeywords;

			bool bResult = false;
			bResult = regKeywords.IsMatch (strFile); //정규식 비교.

			if (false != bResult)//로그파일 형식이 맞으면 시간을 구한다. 
			{
				string strDate = strFile.Substring (nLength - 14, 10);
				try
				{
					dateTemp = Convert.ToDateTime (strDate);
				}
				catch
				{
					return false;
				}
			}
			return bResult;
		}
		#endregion

		#region private 함수.
		/*----------------------------------------------------------------------------------
		 * 로그종류: 정보, 경고, 오류
		 * 로그파일 내용형태
		 * yyyy-MM-dd HH.mm.ss\t오류\t내용
		 -----------------------------------------------------------------------------------*/
		private static void WriteLogFile (string strLogContents)
		{
			StreamWriter writer = null;
			try
			{
				writer = new StreamWriter (LogFilePath, true, Encoding.Default);
				writer.Write (strLogContents);
				writer.WriteLine ();
			}
			catch
			{
			}
			finally
			{
				if (null != writer)
				{
					writer.Close ();
					writer.Dispose ();
				}
			}
		}

		/// <summary>
		/// Object Value를 string으로 처리한다. 
		/// Object 값은 배열일수 있다. 
		/// </summary>
		/// <param name="p"></param>
		/// <returns></returns>
		private static string GetObjectValue (object value)
		{
			string strResult = "";
			if (null == value)
			{ }
			else if (value is object[])
			{
				StringBuilder sb = new StringBuilder ();
				foreach (object objValue in (object[])value)
				{
					if (null == objValue)
						sb.Append ("null");
					else
						sb.Append (objValue.ToString ());
					sb.Append (",");
				}
				sb.Remove (sb.Length - 1, 1);
				strResult = sb.ToString ();
			}
			else if (value is ICollection)
			{
				StringBuilder sb = new StringBuilder ();
				foreach (object objValue in (ICollection)value)
				{
					if (null == objValue)
						sb.Append ("null");
					else
						sb.Append (objValue.ToString ());
					sb.Append (",");
				}
				sb.Remove (sb.Length - 1, 1);
				strResult = sb.ToString ();
			}
			else
				strResult = value.ToString ();

			return strResult;
		}

		/// <summary>
		/// 한문장으로 변환
		/// </summary>
		/// <param name="contents"></param>
		/// <returns></returns>
		private static string GetLinesFromContents (string contents)
		{
			if (null == contents)
				return "null";
			if (string.IsNullOrEmpty (contents))
				return "";
			if (0 > contents.IndexOf ('\n'))
				return "\t\t\t\t"+contents.TrimStart ();

			StringBuilder sbResult = new StringBuilder ("\t\t\t\t");
			string[] strContentLines = contents.Split (new string[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries);
			int nCount = strContentLines.Length;
			
			for (int i = 0; i < nCount; i++)
			{
				sbResult.Append (strContentLines[i].Trim ());
				if (i < nCount)
					sbResult.Append (Environment.NewLine + "\t\t\t\t");
			}
			return sbResult.ToString ();
		}

		/// <summary>
		/// 예외클래스에서 정보 뽑아내기.
		/// </summary>
		/// <param name="sbLog"></param>
		/// <param name="exception"></param>
        private static void GetLogContent(ref StringBuilder sbLog, System.Exception exception)
		{
			if (null == exception)
				return;

			sbLog.AppendFormat ("\t\t\t예외 타입:\r\n\t\t\t\t{0}\r\n", exception.GetType ().Name);
			sbLog.AppendFormat ("\t\t\t메시지:\r\n{0}\r\n", GetLinesFromContents(exception.Message));
			sbLog.AppendFormat ("\t\t\t스택추적:\r\n{0}\r\n", GetLinesFromContents (exception.StackTrace));
			if (0 < exception.Data.Count)
			{
				sbLog.AppendLine ("\t\t\t중요 테이터: ");
				foreach (DictionaryEntry entry in exception.Data)
				{
					sbLog.AppendFormat ("\t\t\t\t{0}={1}\r\n", entry.Key, GetObjectValue (entry.Value));
				}
			}
			if (null != exception.InnerException)
			{
				sbLog.Append ("\t\t\tInnerException 내용:\r\n");
				GetLogContent (ref sbLog, exception.InnerException);
			}
		}
		#endregion
	}
}
