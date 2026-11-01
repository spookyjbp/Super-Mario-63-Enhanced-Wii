onClipEvent(load){
   staramount = 4;
   if(_root.Star[1] == true)
   {
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "Wow!!";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Happy";
      M2Txt = "I can\'t believe you beat King Bob-Omb in a Battle!!";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talking";
      M3Txt = "It looks so hard even REACHING him!";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Happy";
      M4Txt = "Let alone pwning him, like you did!";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Smile";
      M5Txt = "......";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Happy";
      M6Txt = "...You\'re my Hero!";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
   }
   else
   {
      this.swapDepths(0);
      this.removeMovieClip();
   }
}
