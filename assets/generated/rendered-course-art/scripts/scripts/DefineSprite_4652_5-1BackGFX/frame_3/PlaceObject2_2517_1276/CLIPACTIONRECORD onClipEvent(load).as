onClipEvent(load){
   frame = "idle";
   Mood = "Mad";
   M1Moo = "Talk";
   M1Txt = ".......Oh Hi!  Are you a new recruit?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Confused";
   M2Txt = ".......You aren\'t?   Well then, go inside and talk to General Todd.";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = ".......He\'ll give you something to do..... because right now, YOU\'RE DISTRACTING ME!!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
}
