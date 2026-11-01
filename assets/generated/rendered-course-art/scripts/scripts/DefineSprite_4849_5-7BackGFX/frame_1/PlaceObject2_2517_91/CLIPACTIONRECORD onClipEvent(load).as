onClipEvent(load){
   if(_root.Starnum !== 5)
   {
      this._y = 2000;
   }
   staramount = 0;
   count2 = 0;
   frame = "walk";
   Y = this._y;
   sin = 0;
   OEF = function()
   {
      count2++;
      sin += 0.05;
      this._y = Y + Math.sin(sin) * 5;
      if(count2 > 32)
      {
         count2 = 0;
         _xscale = _xscale * -1;
      }
   };
   Mood = "Happy";
   M1Moo = "Talk";
   M1Txt = "......Nope, the secret\'s not on this wall!";
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
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Shocked";
   M3Txt = "....................<b>Wait a minute</b>!!!  If I\'m underwater, then <i>how</i> can I talk?!?";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = undefined;
   M4Txt = undefined;
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Talk";
   M5Txt = "I\'m not really drowning;  You see...";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
   M6Fra = "idle";
   M6Moo = "Sad";
   M6Txt = "I\'m not even a Toad!!! .......I\'m just disguised as one....... I\'m really a.......";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
   M7Fra = "walk";
   M7Moo = "Happy";
   M7Txt = ".......a Platypus!!!!!.........Yeah..... I\'m a Platypus in a Toad Costume!!!";
   M7Sta = function()
   {
      return true;
   };
   M7End = function()
   {
      return BP();
   };
}
