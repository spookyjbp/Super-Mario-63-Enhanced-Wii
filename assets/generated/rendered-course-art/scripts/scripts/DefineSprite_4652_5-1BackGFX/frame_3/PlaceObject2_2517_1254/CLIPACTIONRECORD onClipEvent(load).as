onClipEvent(load){
   frame = "idle";
   Mood = "Mad";
   M1Moo = "Talk";
   M1Txt = ".......Right now, we\'re in the middle of a War;  can\'t you see?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = ".......If you need something, please talk to our General inside this Fortress.";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
}
