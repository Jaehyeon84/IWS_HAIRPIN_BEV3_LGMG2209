using System;
using System.Data.SqlClient;
using System.Threading;
using System.Data;
using System.Collections.Generic;

namespace HuVision
{
    public class DBQry
    {
        public delegate void ConnectEventHandler(string conn);
        public event ConnectEventHandler Connected;
        public event ConnectEventHandler DisConnected;

        private SqlConnection sqlConnection = new SqlConnection();
        private SqlCommand sqlCommand = new SqlCommand();

        private string conn = string.Empty;
        public bool isConnect = false;
        private bool isThreadStop = false;

        private SqlDataAdapter sqlDataAdapter = new SqlDataAdapter();

        private Thread thread;

        public void SetInfo(string ipAddress, string dataCatagory, string userID, string userPW)
        {
            conn = "Data Source=" + ipAddress + ";Initial Catalog=" + dataCatagory + ";User ID=" + userID + ";Password=" + userPW;
        }

        public void CheckConnected()
        {
            if (thread == null)
            {
                thread = new Thread(new ThreadStart(Worker));
                thread.IsBackground = true;
                thread.Start();
            }
        }

        public void Worker()
        {
            while (true)
            {
                if (isThreadStop)
                    break;

                if (!isConnect)
                    Connect();

                Thread.Sleep(1000);
            }
        }

        public bool Connect()
        {
            try
            {
                sqlConnection = new SqlConnection(conn);
                sqlConnection.Open();
                sqlCommand = new SqlCommand();
                sqlCommand.Connection = sqlConnection;
                sqlCommand.CommandType = System.Data.CommandType.Text;

                isConnect = true;

                if (Connected != null)
                    Connected(conn);

                LogManager.WriteLog(LogType.Information, "DB Connect");

                // 3개월 이상 지난 데이터 삭제
                DateTime dt = DateTime.Now.AddMonths(-3);
                ApplyQry(string.Format("DELETE FROM t_Jugement WHERE f_DateTime < '{0}'", dt.ToString("yyyy-MM-dd HH:mm:ss")));
                ApplyQry(string.Format("DELETE FROM t_BefInsp WHERE f_DateTime < '{0}'", dt.ToString("yyyy-MM-dd HH:mm:ss")));
                ApplyQry(string.Format("DELETE FROM t_AftInsp WHERE f_DateTime < '{0}'", dt.ToString("yyyy-MM-dd HH:mm:ss")));
                ApplyQry(string.Format("DELETE FROM t_Error WHERE f_DateTime < '{0}'", dt.ToString("yyyy-MM-dd HH:mm:ss")));

                return true;
            }
            catch
            {
                if (DisConnected != null)
                    DisConnected(conn);

                return false;
            }
        }

        public string ApplyQry(string qry)
        {
            if (!isConnect)
                return string.Empty;

            try
            {
                sqlCommand.CommandText = qry;
                //if (sqlCommand.ExecuteNonQuery() == 0)
                //    return false;

                sqlCommand.ExecuteNonQuery();
                return string.Empty;
            }
            catch (Exception e)
            {
                //LogManager.WriteLog(LogType.Error, e.Message + ": " + qry);
                Close();
                return e.Message;
            }
        }

        public bool ParameterApplyQry(string qry, string[] parameterName, List<byte[]> paramterValue)
        {
            if (!isConnect)
                return false;

            try
            {
                SqlCommand parameterCommand = new SqlCommand();
                parameterCommand.Connection = sqlConnection;
                parameterCommand.CommandType = System.Data.CommandType.Text;
                parameterCommand.CommandText = qry;
                for (int i = 0; i < parameterName.Length; i++)
                    parameterCommand.Parameters.Add(parameterName[i], SqlDbType.Binary).Value = paramterValue[i];

                if (parameterCommand.ExecuteNonQuery() == 0)
                    return false;

                return true;
            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, e.Message + ": " + qry);
                Close();
                return false;
            }
        }

        public DataTable SelectData(string qry)
        {
            if (!isConnect)
                return null;

            try
            {
                DataSet dataSet = new DataSet();

                sqlCommand.CommandText = qry;
                sqlDataAdapter.SelectCommand = sqlCommand;
                sqlDataAdapter.SelectCommand.CommandTimeout = 5;

                sqlDataAdapter.Fill(dataSet);

                return dataSet.Tables[0];
            }
            catch (Exception e)
            {
                LogManager.WriteLog(LogType.Error, e.Message + ": " + qry);
                Close();
                return null;
            }
        }

        private void Close()
        {
            if (sqlCommand != null)
            {
                sqlCommand.Dispose();
                sqlCommand = null;
            }

            if (sqlConnection.State == System.Data.ConnectionState.Open)
            {
                sqlConnection.Close();
                sqlConnection.Dispose();
                sqlConnection = null;
            }

            isConnect = false;

            if (DisConnected != null)
                DisConnected(conn);

            LogManager.WriteLog(LogType.Information, "DB DisConnect");
        }

        public void Exit()
        {

            isThreadStop = true;

            if (thread != null)
                thread = null;

            Close();
        }
    }
}
