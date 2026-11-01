onClipEvent(load){
   staramount = 22;
   count2 = 0;
   speed = 2;
   OEF2 = function()
   {
      _X = _X + speed;
      count2++;
      if(count2 > 32)
      {
         count2 = 0;
         speed *= -1;
      }
      _xscale = speed * 50;
   };
   frame = "walk";
   Mood = "Mad";
   M1Moo = "Crazy";
   M1Txt = "....Gaah!!!!....";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = "Why did Princess Peach forget to put even 1 Bathroom in this Castle!?";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
}
