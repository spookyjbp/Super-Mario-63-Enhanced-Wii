onClipEvent(load){
   count2 = 0;
   speed = 2;
   OEF2 = function()
   {
      _X = _X + speed;
      count2++;
      if(count2 > 60)
      {
         count2 = 0;
         speed *= -1;
      }
      _xscale = speed * 50;
   };
   frame = "walk";
   Mood = "Mad";
   M1Moo = "Crazy";
   M1Txt = "....Oh No!!!!....";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = "....The Left Flank just got hit by a Bob-Omb!!!!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = "..........................!!!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
}
