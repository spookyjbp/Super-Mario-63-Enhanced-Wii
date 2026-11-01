onClipEvent(load){
   staramount = 3;
   _xscale = -100;
   frame = "idle";
   Mood = "Smile";
   M1Moo = "Happy";
   M1Txt = "Have you been to the <b>Tidal Isles</b> yet?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Happy";
   M2Txt = "It\'s a lovely place, with beaches, waves, sunken treasure, and lots of tropical fish!!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
}
