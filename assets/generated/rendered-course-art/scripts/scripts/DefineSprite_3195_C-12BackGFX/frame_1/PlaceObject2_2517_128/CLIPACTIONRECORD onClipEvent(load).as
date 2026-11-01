onClipEvent(load){
   staramount = 1;
   if(random(2) == 0)
   {
      Mood = "Sad";
      M1Moo = "Talk";
      M1Txt = "....I\'ve been waiting here for about 12 hours...";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Mad";
      M2Txt = "... But this room apparently only is Night-time....";
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
   }
   else
   {
      this._y = 500;
   }
}
