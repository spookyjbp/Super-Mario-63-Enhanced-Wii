onClipEvent(load){
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = "This is our Lava-Wheel!!";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Happy";
   M2Txt = "The Current of the Lava spins it so that we can generate power!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = "However, after Bowser invaded the castle, 5 Silver Stars have gotten themselves trapped within our Lava Wheel.";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Talk";
   M4Txt = "If you can get them all, we\'ll let you keep them!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Talk";
   M5Txt = "But <b>Beware:</b>  The Lava Wheel can easily pull you into the lava, resulting in an <b>Instant Death.....</b>";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
}
