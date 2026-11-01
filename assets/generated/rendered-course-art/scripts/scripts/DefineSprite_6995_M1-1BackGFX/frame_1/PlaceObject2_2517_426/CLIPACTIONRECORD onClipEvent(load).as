onClipEvent(load){
   staramount = 0;
   if(random(false))
   {
      _Y = 500;
   }
   Mood = "Shocked";
   M1Moo = "Shocked";
   M1Txt = ".......<i>Whew!</i> ...........It\'s <i>so</i> <b>hot</b> here!...";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Crazy";
   M2Txt = ".......<i>I can barely stand it!</i>........";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
}
