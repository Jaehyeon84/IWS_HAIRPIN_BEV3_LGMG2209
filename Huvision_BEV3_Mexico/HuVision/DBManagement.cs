using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HuVision
{
    public class DBManagement
    {
        private DBQry dbQry = new DBQry();

        private string befLotID = string.Empty;
        private string aftLotID = string.Empty;

        private string errorLotID = string.Empty;

        private string befDateTime = string.Empty;
        private UInt64 befDataIndex = 0;

        private bool isUse = false;

        public void Init()
        {
            IniManagement iniManagement = IniManagement.Instance();
            isUse = iniManagement.iniModel.DB_Use;
            if (isUse)
            {
                dbQry.SetInfo(iniManagement.iniModel.DB_IP, iniManagement.iniModel.DB_Catagory, iniManagement.iniModel.DB_ID, iniManagement.iniModel.DB_PW);
                dbQry.CheckConnected();
            }
        }

        public void JugementInsert(string lotID)
        {
            if (!isUse)
                return;

            if(string.IsNullOrWhiteSpace(lotID))
            {
                LogManager.WriteLog(LogType.Warning, "There is no LotID.");
                return;
            }

            // Error가 발생 한 LotID는 처리하지 않음.
            if (errorLotID == lotID)
            {
                LogManager.WriteLog(LogType.Warning, "The LotID is the LotID where the error occurred. " + lotID);
                return;
            }
            errorLotID = string.Empty;

            // 후검사 후 전검사 진행 시 저장하지 않음.
            if (aftLotID == lotID)
            {
                LogManager.WriteLog(LogType.Warning, "LotID that has already been inspected after welding. " + lotID);
                return;
            }

            aftLotID = string.Empty;

            try
            {
                DateTime dateTime = DateTime.Now;
                UInt64 dataIndex = (UInt64)(DateTime.Now.Subtract(new TimeSpan(2015 * 365, 0, 0, 0)).Ticks / 10000000);
                befDataIndex = dataIndex;
                befDateTime = dateTime.ToString("yyyy-MM-dd HH:mm:ss");

                string qry = string.Format("INSERT INTO t_Jugement VALUES ({0},{1},'{2}','{3}','',0,0,0,0,'')",
                    dataIndex, dataIndex, lotID, befDateTime);

                if (InsertQry(qry, lotID))
                    BefInspInsert(befDateTime, lotID);  // 전검사 데이터 저장                    

                befLotID = lotID;
            }
            catch (Exception e)
            {
                errorLotID = lotID;
               InserError(lotID, "BefInspInsert Error: " + e.Message);
            }
        }

        private void BefInspInsert(string dateTime, string lotID)
        {
            try
            {
                string path = Application.StartupPath + "\\lastPst.dat";
                if (!File.Exists(path))
                {
                    LogManager.WriteLog(LogType.Warning, "lastPst.dat The file does not exist.");
                    return;
                }

                string target = Application.StartupPath + "\\tmp_insert_lastPst.dat";
                File.Copy(path, target, true);

                using (StreamReader sr = new StreamReader(target, Encoding.UTF8))
                {
                    string qry = "INSERT INTO t_BefInsp VALUES ";
                    bool isFirst = true;
                    while (!sr.EndOfStream)
                    {
                        string[] tokken = sr.ReadLine().Split(',');
                        if (isFirst)
                        {
                            isFirst = false;
                            continue;
                        }

                        // Slot,Layer,Result,1H,2H,3H,1A,2A,AX,AY,BX,BY,C,Jump,Theta,Tmp1,Tmp2,Tmp3
                        qry += string.Format("({0}, '{1}', '{2}', {3}, '{4}', {5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17}),",
                            befDataIndex, lotID, dateTime,                      // Slot, Layer, Result
                            tokken[0], tokken[1], tokken[2],                    // 1H, 2H, 3H
                            tokken[3], tokken[4],                               // 1A, 2A
                            tokken[5], tokken[6], tokken[7], tokken[8],         // ax, ay, bx, by
                            tokken[9], tokken[10], tokken[11],                  // c, jump, theta
                            tokken[12], tokken[13], tokken[14]);                // tmp1, tmp2, tmp3                       
                    }

                    InsertQry(qry.Remove(qry.Length - 1), lotID);
                }

                File.Delete(target);
            }
            catch(Exception e)
            {
                errorLotID = lotID;
                InserError(lotID, "BefInspTableInser Error: " + e.Message);
            }
        }

        public void AftInspInsert(string dateTime, string lotID, int pass, int rework, int fail, int result, string csvPath, string imgPath)
        {
            if (!isUse)
                return;

            if (string.IsNullOrWhiteSpace(lotID))
            {
                LogManager.WriteLog(LogType.Warning, "There is no LotID.");
                return;
            }

            // Error가 발생 한 LotID는 처리하지 않음.
            if (errorLotID == lotID)
            {
                LogManager.WriteLog(LogType.Warning, "The LotID is the LotID where the error occurred. " + lotID);
                return;
            }
            errorLotID = string.Empty;

            // 전검사 LotID와 일치하지 않으면 Pass
            if (befLotID != lotID)
            {
                LogManager.WriteLog(LogType.Warning, "It does not match with LotID of inspection before welding. " + lotID);
                return;
            }

            try
            {
                if(aftLotID == lotID)  
                {
                    // 중복검사
                    UInt64 dataIndex = (UInt64)(DateTime.Now.Subtract(new TimeSpan(2015 * 365, 0, 0, 0)).Ticks / 10000000);
                    string qry = string.Format("INSERT INTO t_Jugement VALUES ({0},{1},'{2}','{3}','{4}',{5},{6},{7},{8},'{9}')",
                    dataIndex, befDataIndex, lotID, befDateTime, dateTime, 
                    pass, rework, fail, result, imgPath);

                    if (InsertQry(qry, lotID))
                        AftInspTableInsert(dataIndex, dateTime, lotID, csvPath);
                }
                else                    
                {
                    // 후검사 첫 검사 Update
                    string qry = string.Format(@"UPDATE t_Jugement SET f_AftTime = '{0}', f_PassCount = {1}, f_ReworkCount = {2}, f_FailCount = {3},
                                                 f_AftResult = {4}, f_ImagePath = '{5}' WHERE f_DataIndex = {6}",
                                                dateTime, pass, rework, fail, result, imgPath, befDataIndex);

                    if (InsertQry(qry, lotID))
                        AftInspTableInsert(befDataIndex, dateTime, lotID, csvPath);
                }

                aftLotID = lotID;
            }
            catch(Exception e)
            {
                errorLotID = lotID;
                InserError(lotID, "AftInspInsert Error: " + e.Message);
            }
        }

        private void AftInspTableInsert(UInt64 dataIndex, string dateTime, string lotID, string csvPath)
        {
            try
            {
                if (!File.Exists(csvPath))
                {
                    LogManager.WriteLog(LogType.Warning, csvPath + " The file does not exist.");
                    return;
                }

                string target = Application.StartupPath + "\\tmp_insert_aftInsp.dat";
                File.Copy(csvPath, target, true);

                using (StreamReader sr = new StreamReader(target, Encoding.UTF8))
                {
                    bool isFirst = true;
                    string qry = "INSERT INTO t_AftInsp VALUES ";
                    while (!sr.EndOfStream)
                    {
                        string[] tokken = sr.ReadLine().Split(',');
                        if (isFirst)
                        {
                            isFirst = false;
                            continue;
                        }

                        qry += string.Format("({0}, '{1}', '{2}', {3}, '{4}', {5},{6},{7},'{8}','{9}',{10},{11},{12},{13},{14},{15},'{16}'),",
                            dataIndex, lotID, dateTime,
                            tokken[0], tokken[1], tokken[2], tokken[3], tokken[4],    // Slot, Layer, Result, Probability, OldResult
                            tokken[5], tokken[6], tokken[7],                          // InnerHeight, OuterHeight, CenterHeight
                            tokken[8], tokken[9], tokken[10],                         // InnerWidth, CenterWidth, OuterWidth
                            tokken[11], tokken[12], "");                              // Area, Type, ImgPath
                    }

                    InsertQry(qry.Remove(qry.Length - 1), lotID);
                }

                File.Delete(target);
            }
            catch (Exception e)
            {
                errorLotID = lotID;
                InserError(lotID, "BefInspTableInser Error: " + e.Message);
            }
        }

        private bool InsertQry(string qry, string lotID)
        {
            string errorMsg = dbQry.ApplyQry(qry);
            if (errorMsg != string.Empty)
            {
                errorLotID = lotID;
                InserError(lotID, errorMsg);
                return false;
            }

            return true;
        }

        private void InserError(string lotID, string msg)
        {
            LogManager.WriteLog(LogType.Error, msg);

            string qry = string.Format("INSERT INTO t_Error VALUES ('{0}', '{1}', '{2}')",
                DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"), lotID, msg);

            string errorMsg = dbQry.ApplyQry(qry);

            if (errorMsg != string.Empty)
                LogManager.WriteLog(LogType.Error, "InserError: " + errorMsg);
        }

        public void Close()
        {
            dbQry.Exit();
        }
    }
}
