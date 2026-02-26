using HuVision.Model;
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace HuVision
{
    public class IniManagement
    {
        [DllImport("Kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);
        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section, string key, string val, string filePath);

        private static IniManagement instance;
        protected IniManagement() { }

        public static IniManagement Instance()
        {
            if (instance == null)
                instance = new IniManagement();

            return instance;
        }

        public IniModel iniModel = new IniModel();
        public bool OpenIniModel()
        {
            return OpenIni(iniModel, Application.StartupPath + "\\config.ini");
        }

        public void SaveIniModel()
        {
            SaveIni(iniModel, Application.StartupPath + "\\config.ini");
        }

        private bool OpenIni(object cls, string iniPath)
        {
            System.Reflection.PropertyInfo[] propertyInfos = cls.GetType().GetProperties();
            for (int i = 0; i < propertyInfos.Length; i++)
            {
                try
                {
                    propertyInfos[i].SetValue(cls, Convert.ChangeType(GetData(iniPath, propertyInfos[i].Name, propertyInfos[i].PropertyType), propertyInfos[i].PropertyType), null);
                }
                catch (Exception e)
                {
                    LogManager.WriteLog(LogType.Error, "OpenIni: " + propertyInfos[i].Name + " " + e.Message);
                    //return false;
                }
            }

            return true;
        }

        private void SaveIni(object cls, string iniPath)
        {
            System.Reflection.PropertyInfo[] propertyInfos = cls.GetType().GetProperties();
            for (int i = 0; i < propertyInfos.Length; i++)
            {
                string[] tokken = propertyInfos[i].Name.Split('_');
                WritePrivateProfileString(tokken[0], tokken[1], propertyInfos[i].GetValue(cls, null).ToString(), iniPath);
            }
        }

        private object GetData(string iniPath, string iniData, Type type)
        {
            try
            {
                string[] tokken = iniData.Split('_');

                StringBuilder stringBuilder = new StringBuilder(255);
                GetPrivateProfileString(tokken[0], tokken[1], "", stringBuilder, 255, iniPath);
                if (string.IsNullOrWhiteSpace(stringBuilder.ToString()))
                {
                    if (type == typeof(Boolean))
                        return false;

                    return "0";
                }

                return stringBuilder.ToString();
            }
            catch
            {
                return "0";
            }
        }
    }
}
