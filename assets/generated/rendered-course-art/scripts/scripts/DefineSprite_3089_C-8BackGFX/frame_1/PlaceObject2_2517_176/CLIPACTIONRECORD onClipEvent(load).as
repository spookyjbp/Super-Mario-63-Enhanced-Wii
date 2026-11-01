onClipEvent(load){
   staramount = 0;
   _X = _X + random(400);
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
   Mood = "Shocked";
   M1Moo = "Shocked";
   M1Txt = "....*gulp*...............*gulp*................*gulp*...........";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Shocked";
   M2Txt = "...........Help Me I\'m Drowning!!!!!!!.............*gulp*.........";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Shocked";
   M3Txt = "..........*gulp*..........*gulp*..............*gulp*..................";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Happy";
   M4Txt = "...Just Kidding!!!!";
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
   M8Fra = "idle";
   M8Moo = "Talk";
   M8Txt = "......I\'m here to investigate the water quality at the Castle.....";
   M8Sta = function()
   {
      return true;
   };
   M8End = function()
   {
      return BP();
   };
   M9Fra = "walk";
   M9Moo = "Happy";
   M9Txt = ".....It\'s part of my role at the Aquatics Sanitary Society!!";
   M9Sta = function()
   {
      return true;
   };
   M9End = function()
   {
      return BP();
   };
}
