onClipEvent(load){
   staramount = 1;
   if(random(2) == 0)
   {
      Mood = "Smile";
      M1Moo = "Happy";
      M1Txt = "I just <i>Love</i> this large, panoramic window!";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Happy";
      M2Txt = "....<i>Just</i> being able to <i>stand</i> here makes my life <i>feel complete</i>...";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M4Moo = "Talk";
      M4Txt = "Hungry Hungry Hungry Hungry";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Talk";
      M5Txt = "Hungry Hungry Hungry Hungry Hungry";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Talk";
      M6Txt = "Hungry Hungry Hungry Hungry Hungry Hungry";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M9Moo = "Mad";
      M9Txt = "I thought you were a nice person!!";
      M9Sta = function()
      {
         return true;
      };
      M9End = function()
      {
         return BP();
      };
      M9F = function()
      {
      };
      M10Moo = "Sad";
      M10Txt = "And then you come here and Destroyed all my Dreams!";
      M10Sta = function()
      {
         return true;
      };
      M10End = function()
      {
         return BP();
      };
      M11Moo = "Crazy";
      M11Txt = "DESTROYED IT!!!!";
      M11Sta = function()
      {
         return true;
      };
      M11End = function()
      {
         return BP();
      };
      M11F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay("noo");
      };
      M12Moo = "Mad";
      M12Txt = "......";
      M12Sta = function()
      {
         return true;
      };
      M12End = function()
      {
         return BP();
      };
      M12F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay("talk");
      };
      M13Moo = "Mad";
      M13Txt = "...Gee, Thanks for ruining my life...";
      M13Sta = function()
      {
         return true;
      };
      M13End = function()
      {
         return BP();
      };
      M13F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay(1);
         this._xscale *= -1;
         Mood = "Mad";
         StartMood = "Mad";
         M1Moo = "Mad";
         M1Txt = "......Go Away!!";
         M1Sta = function()
         {
            return BP();
         };
         M1End = function()
         {
            return BP();
         };
         M2Moo = undefined;
         M2Txt = undefined;
         M2Sta = null;
         M2End = null;
      };
   }
   else
   {
      this._y = 500;
   }
}
