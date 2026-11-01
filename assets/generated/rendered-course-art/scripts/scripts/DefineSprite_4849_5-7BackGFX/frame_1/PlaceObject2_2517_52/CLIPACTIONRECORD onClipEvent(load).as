onClipEvent(load){
   staramount = 0;
   frame = "idle";
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = ".......You\'re probably wondering how we got Water to float on the sides of the walls, right?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = ".......................";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = ".......Aren\'t you surprised by how we got Water to blatantly ignore Gravity?!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Crazy";
   M4Txt = "....................What?!?!  You think it is easy altering the Laws of Physics, like Gravity?!?!?  How \'bout you try?!?";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
