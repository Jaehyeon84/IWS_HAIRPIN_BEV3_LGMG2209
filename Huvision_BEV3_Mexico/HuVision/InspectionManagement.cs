using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

//#pragma comment(linker, "/DELAYLOAD:\\lib\\WeldAfterInspect3D.dll")

namespace HuVision
{

    public class InspectionManagement
    {

        //[DllImport("WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void hbsSetWeldAfterFile(char[] filename, char[] topPinFile);

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int hbsDoProcess();

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr hbsGetResults(); // IntPtr Resutn double*

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int hbsGetNumSlot();

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int hbsGetNumWeldLayer();

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void hbsShowProcessImage(char[] filename, bool bshow);

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void hbsReloadConfig();

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void hbsInitialize();

        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void hbsExit();

        // IntPtr Resutn float*
        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr hbsGetImage(ref int iW, ref int iH);
        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr hbsGetProfileR(int idx, ref int nData, ref int sx, ref int sy, ref int ex, ref int ey);
        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr hbsGetProfileWeldR(int idx, ref int nData);
        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr hbsGetProfileWeldCorrectR(int idx, ref int nData);
        [DllImport("\\lib\\WeldAfterInspect3D.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr hbsGetProfileT(int idx, ref int nData, ref int sx, ref int sy, ref int ex, ref int ey);

        private int size = (48 * 2) * 11;

        public void Initialize()
        {
            hbsInitialize();
        }

        public void Exit()
        {
            hbsExit();
        }

        public void ReloadConfig()
        {
            hbsReloadConfig();
        }

        public int DoProcess(string afterImagePath, string beforeImagePath)
        {
            hbsSetWeldAfterFile(afterImagePath.ToArray(), beforeImagePath.ToArray());
            System.Threading.Thread.Sleep(100);
            return hbsDoProcess();
        }

        public double[] ImageProcess(string saveImagePath)
        {
            hbsShowProcessImage(saveImagePath.ToArray(), false);

            double[] pRes = new double[size];
            Marshal.Copy(hbsGetResults(), pRes, 0, size);

            return pRes;
        }

        public float[] GetProfileR(int index)
        {
            try
            {
                int count = 0, sx = 0, sy = 0, ex = 0, ey = 0;

                IntPtr tmpPtr = hbsGetProfileR(index, ref count, ref sx, ref sy, ref ex, ref ey);
                float[] profileDataArr = new float[count];
                Marshal.Copy(tmpPtr, profileDataArr, 0, count);

                return profileDataArr;
            }
            catch
            {
                return null;
            }
            
        }

        public float[] GetProfileT(int index)
        {
            try
            {
                int count = 0, sx = 0, sy = 0, ex = 0, ey = 0;

                IntPtr tmpPtr = hbsGetProfileT(index, ref count, ref sx, ref sy, ref ex, ref ey);
                float[] profileDataArr = new float[count];
                Marshal.Copy(tmpPtr, profileDataArr, 0, count);

                return profileDataArr;
            }
            catch
            {
                return null;
            }
        }

        //public double[] Processing(string afterImagePath, string beforeImagePath, string saveImagePath)
        //{
        //    hbsSetWeldAfterFile(afterImagePath.ToArray(), beforeImagePath.ToArray());
        //    hbsDoProcess();

        //    int nWeldLayer = hbsGetNumWeldLayer();
        //    int nSlot = hbsGetNumSlot();

        //    double[] pRes = new double[size];
        //    Marshal.Copy(hbsGetResults(), pRes, 0, size);

        //    hbsShowProcessImage(saveImagePath.ToArray(), false);

        //    return pRes;
        //}
    }
}
