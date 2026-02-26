using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HuVision.Model
{
    public class InspectionModel
    {
        public int WeldAfterInspect_NumberofSlots { get; set; }
        public int WeldAfterInspect_NumberofLayers { get; set; }
        public int WeldAfterInspect_UseDeepLearning { get; set; }

        // Type 7
        public int WeldAfterInspect_UseArea {get;set;}                      // Area(Score) 사용여부
        public double WeldAfterInspect_JudgementCriteriaArea { get; set; }  // Socre 기준 값

        // Type 17
        public int WeldAfterInspect_UsePinHeightDiff {get;set;}                   // 핀 간 높이 편차 검사
        public double WeldAfterInspect_JudgementCriteriaHeightDiff { get; set; }  // 편차 기준 값

        // Type 6
        public int WeldAfterInspect_UseType6 {get;set;}                          // 용접 전 후 높이 편차 검사
        public double WeldAfterInspect_JudgementCriteriaDepthDiff { get; set; }  // 전 후 편차 기준 값 

        // 비드폭 검사 여부
        public int WeldAfterInspect_UseBeadWidth {get;set;}                      // 비드 폭 검사여부
        public double WeldAfterInspect_JudgementCriteriaBeadWidth { get; set; }  // 비드 폭 기준        
    }
}
