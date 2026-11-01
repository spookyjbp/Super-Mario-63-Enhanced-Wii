onClipEvent(load){
   staramount = 0;
   if(random(2) == 0)
   {
      _Y = 500;
   }
   Mood = "Shocked";
   M1Moo = "Shocked";
   M1Txt = "........................<b>So</b>.......<b>High</b>.......<b>Up</b>.......";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = ".......<i>Why Did I even take that Sling Star!!</i>........";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
}
