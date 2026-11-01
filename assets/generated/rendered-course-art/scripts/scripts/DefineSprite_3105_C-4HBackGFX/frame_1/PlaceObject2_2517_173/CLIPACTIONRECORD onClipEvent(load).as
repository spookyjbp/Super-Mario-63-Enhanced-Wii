onClipEvent(load){
   if(_root.TotalStars <= 0 && _root.Star[39] == false)
   {
      staramount = 0;
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "<b><i>Eddie: </i></b>Up this elevator;  actually, you know what, take the Sling Star over there.";
      M1Sta = function()
      {
         if(count2++ > 5)
         {
            return true;
         }
         return false;
      };
      standnext = false;
      M1End = function()
      {
         return BP();
      };
      count2 = 0;
      M2Moo = "Happy";
      M2Txt = "<b><i>Eddie: </i></b>It\'s faster... Don\'t worry, we had it installed as a suprise for the Party...";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M2F = function()
      {
         OEF2 = function()
         {
            _root.Camcenter = _root.Course.FrontGFX.ss1;
         };
         M1Sta = function()
         {
            return BP();
         };
      };
      M3Moo = "Crazy";
      M3Txt = "<b><i>Eddie: </i></b>..... That is, before Bowser came.";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Happy";
      M4Txt = "<b><i>Eddie: </i></b>Just jump into it and press X to Spin around and blast up to the top!";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M4F = function()
      {
         OEF2 = null;
         count2 = 0;
         M1Sta = function()
         {
            if(count2++ <= 10)
            {
               return false;
            }
            this._y = 500;
            M1Sta = undefined;
         };
      };
   }
   else
   {
      this._y = 400;
   }
}
