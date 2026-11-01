onClipEvent(load){
   if(random(3) == 0 && (_root.BowserKey2 == true || _root.CheatCastleUnlock == true))
   {
      Mood = "Mad";
      M1Moo = "Crazy";
      M1Txt = "<b>NNNOOOOOOOO!!!!!!!!!!</b>";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Fra = "walk";
      M2Moo = "Crazy";
      M2Txt = "<b>What is Bowser doing with that Meteor of his!!?!?!?</b>";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Crazy";
      M3Txt = "<b>We\'re All Gonna DIE!!!</b>";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M5Moo = "Happy";
      M5Txt = "With all the other Toads returning, this place is starting to feel lively again! ";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
   }
   else
   {
      this._y = 500;
   }
}
