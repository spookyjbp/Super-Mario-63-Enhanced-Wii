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
   M1Txt = "....Take that!! Evil Bullies....";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
}
