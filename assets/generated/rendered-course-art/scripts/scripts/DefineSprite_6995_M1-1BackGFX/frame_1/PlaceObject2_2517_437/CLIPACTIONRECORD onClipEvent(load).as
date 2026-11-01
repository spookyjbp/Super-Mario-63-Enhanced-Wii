onClipEvent(load){
   staramount = 0;
   _xscale = -100;
   Mood = "Confused";
   M1Moo = "Talk";
   M1Txt = "....Our excavational teams haven\'t yet dug up this temple structure here....";
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
   M2Moo = "Confused";
   M2Txt = ".......However, this sand doesn\'t seem to be any <i>normal</i> sand..........";
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
      _xscale = -100;
   };
}
