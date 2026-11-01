onClipEvent(load){
   staramount = 3;
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = "Hello, " + _root.CurrentPlayer;
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Talk";
   M2Txt = "Since the Shine Sprites are starting to return to the Castle, most of the Toads are returning too.";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   if(_root.TotalStars < 20)
   {
      M3Moo = "Talk";
      M3Txt = "Before Bowser came, this place was full of Toads running all over the place.";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Mad";
      M4Txt = "Now it feels empty and secluded.";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
   }
   else
   {
      M3Moo = "Happy";
      M3Txt = "With all the other Toads returning, this place is starting to feel lively again! ";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
   }
}
