onClipEvent(load){
   frame = "idle";
   Mood = "Mad";
   M1Moo = "Talk";
   M1Txt = ".......Because the War ended, I\'m no longer needed to run the Cannons.";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = "......I\'m out of a job!!!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Crazy";
   M3Txt = ".......How \'bout you start another war... just for me????";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
}
