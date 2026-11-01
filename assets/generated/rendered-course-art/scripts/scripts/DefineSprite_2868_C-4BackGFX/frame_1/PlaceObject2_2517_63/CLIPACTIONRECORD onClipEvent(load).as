onClipEvent(load){
   staramount = 0;
   _xscale = -100;
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = "The Level Designer!";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Happy";
   M2Txt = "Hurry!  Jump into it and start creating your VERY OWN LEVELS...";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Happy";
   M3Txt = "And you can even share your levels with other people!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Sad";
   M4Txt = "But don\'t stay too long, Mushroom Kingdom still needs saving!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
