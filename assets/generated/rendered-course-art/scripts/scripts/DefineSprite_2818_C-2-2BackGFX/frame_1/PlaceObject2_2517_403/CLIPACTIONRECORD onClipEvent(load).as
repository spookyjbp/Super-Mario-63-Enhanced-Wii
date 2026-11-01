onClipEvent(load){
   if(_root.StarCoin[61] !== true)
   {
      staramount = 1;
      Mood = "Mad";
      M1Moo = "Talk";
      M1Txt = "........You know..................";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Mad";
      M2Txt = "This room always makes me feel sad.";
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
         this._xscale *= -1;
      };
      M3Moo = "Talk";
      M3Txt = "My best friend, Henry, somehow disappeared in this room,";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Crazy";
      M4Txt = "Over 9,000 Years ago.........";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Talk";
      M5Txt = "...The last thing he said to me was:";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M5F = function()
      {
         count2 = 0;
         _xscale = _xscale * -1;
      };
      M6Moo = "Happy";
      M6Txt = "\"<i> I\'m gonna get some Ice Cream in Snowman\'s Land</i>\"";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M7Moo = "Talk";
      M7Txt = "...........He never returned......";
      M7Sta = function()
      {
         return true;
      };
      M7End = function()
      {
         return BP();
      };
   }
   else
   {
      _xscale = 100;
      Mood = "Mad";
      M1Moo = "Shocked";
      M1Txt = "..........HENRY!!!!!";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M1F = function()
      {
         _xscale = -100;
      };
      M2Moo = "Happy";
      M2Txt = "After all these years, I finally see you again.";
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
         this._xscale = -100;
      };
      M3Moo = "Happy";
      M3Txt = "<b>Henry:</b> <i>Thank you " + _root.CurrentPlayer + " for saving me from being stuck in wall.</i>";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M3F = function()
      {
         this._xscale = 100;
      };
      M4Moo = "Happy";
      M4Txt = "Thank You!!!";
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
         _xscale = -100;
      };
      M5Moo = "Happy";
      M5Txt = "<b>Henry:</b> <i>For saving me, I\'ll let you keep that Star Coin I was trapped with...</i>";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Happy";
      M6Txt = "By collecting enough Star Coins, you can unlock stuff for the Level Designer!!";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M6F = function()
      {
         Mood = "Happy";
         StartMood = "Happy";
         M1Fra = "walk";
         M1Moo = "Happy";
         M1Txt = "Thank You!";
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
}
