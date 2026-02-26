using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HuVision
{
    //public enum FRAME_MSG { MESSAGE, PLC_CONNECT, PLC_DISCONNECT, CLIENT_CONNECT, CLIENT_DISCONNECT }
    public enum LogType { Information, Warning, Error }
    public enum RESPONSE { NONE = 0, OK, NG, ALARM, PASS }
    //public enum INPUT_FRAME { INIT = 0, START, END,  MODE, MODEL_NO, YYYY, MM, DD, HH, mm, ss };
    public enum INPUT_FRAME { LINK = 0, INSP, PLC_MODEL, RCP };
    public enum OUTPUT_FRAME { LINK = 0, INSP };

    public enum OUTPUT_INSPT { START = 1, END, STOP};

    public enum OUTPUT_INSPTCOMPLETE { END = 1 };

    //public enum OUTPUT_FRAME { INIT = 0, START, END, AFT_RESULT };

    public class UserUtil
    {
        /// <summary>
        /// R 값 구하기
        /// </summary>
        /// <param name="dx">평균 X</param>
        /// <param name="x">측정 X</param>
        /// <param name="dy">평균 Y</param>
        /// <param name="y">측정 Y</param>
        /// <returns></returns>
        public static double GetR(double dx, double x, double dy, double y)
        {
            return Math.Truncate(Math.Sqrt(((dx - x) * (dx - x)) + ((dy - y) * (dy - y))) * 100) / 100;
        }

        public static double GetTruncate(double value)
        {
            return Math.Truncate(value * 100) / 100;
        }

        /// <summary>
        /// 극좌표 -> 직교좌표
        /// </summary>
        public static void P2R(double r, double th, out double x, out double y)
        {
            x = r * Math.Cos(th);
            y = r * Math.Sin(th);
        }

        /// <summary>
        /// 직교좌표 -> 극좌표
        /// </summary>
        public static void R2P(double x, double y, out double r, out double th)
        {
            r = Math.Sqrt(x * x + y * y);
            th = Math.Atan2(y, x);
        }
    }
}
