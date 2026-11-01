onClipEvent(load){
   staramount = 0;
   _xscale = -100;
   frame = "walk";
   Mood = "Mad";
   M1Moo = "Talk";
   M1Txt = ".....I keep pressing the UP button, but my character can\'t jump...";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Crazy";
   M2Txt = "Why didn\'t Runouw (the developer) program me to be able to Jump?!?!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = undefined;
   M3Txt = undefined;
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = "....It should really be Final-but-not-really-final-because-we-will-undoubtably-release-more-and-more-sequels-despite-half-the-games-released-are-spin-offs-of-";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Mad";
   M5Txt = "-of-other-previous-f-f-titles-so-that-the-number-that-represents-this-new-game-is-so-large-that-it-even-over-takes-this-name-in-terms-of-length- Fantasy...";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
   M6Moo = "Talk";
   M6Txt = "But none the less, I am still enjoying that game thoroughly, so I have no other complaints...";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
}
