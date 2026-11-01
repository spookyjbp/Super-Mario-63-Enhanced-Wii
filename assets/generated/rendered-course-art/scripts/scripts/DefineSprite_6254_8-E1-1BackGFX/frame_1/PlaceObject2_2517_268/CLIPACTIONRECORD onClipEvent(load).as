onClipEvent(load){
   staramount = 0;
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = ".....Did you know that you could also talk to Toads like me with the Z button?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = undefined;
   M2Txt = undefined;
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = ".... I thought it was the FINAL Fantasy, meaning the LAST...... EVER .......";
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
