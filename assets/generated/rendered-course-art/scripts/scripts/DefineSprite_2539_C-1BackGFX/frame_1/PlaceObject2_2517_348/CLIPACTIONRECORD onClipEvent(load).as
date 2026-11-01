onClipEvent(load){
   if(random(2) == 0 && (_parent.toaddisp8 == "bottom" || _parent.toaddisp8 == undefined))
   {
      _parent.toaddisp8 = "bottom";
      staramount = 12;
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "......Yesterday, I was bored, so I decided to buy that new Final Fantasy game";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Shocked";
      M2Txt = "...FFMMDCCCLXXXVIII.... That is the two-thousand eight-hundred and ninety-eighth game in the series!?!?!";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talk";
      M3Txt = ".... I thought it was the <b>FINAL</b> Fantasy, meaning the LAST...... <i>EVER</i> .......";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Mad";
      M4Txt = "....It should really be Final-but-not-really-final-because-we-will-undoubtably-release-more-and-more-sequels-despite-half-the-";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Mad";
      M5Txt = "-games-released-are-spin-offs-of-other-previous-f-f-titles-so-that-the-number-that-represents-this-new-game-is-so-large-that-it-";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Moo = "Mad";
      M6Txt = "-it-even-overtakes-the-normal-letter-name-of-final-fantasy-in-terms-of-length-and-number-of-characters Fantasy...";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
      M7Moo = "Talk";
      M7Txt = "But none the less, I am still enjoying that game thoroughly, so I have no other complaints...";
      M7Sta = function()
      {
         return true;
      };
      M7End = function()
      {
         return BP();
      };
   }
   else
   {
      _parent.toaddisp8 = "top";
      this._y = 400;
   }
}
