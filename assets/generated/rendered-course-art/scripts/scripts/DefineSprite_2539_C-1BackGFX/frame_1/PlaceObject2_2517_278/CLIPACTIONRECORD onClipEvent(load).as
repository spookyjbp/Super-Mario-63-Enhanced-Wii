onClipEvent(load){
   staramount = 5;
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = "Did you know that by pressing X, you can preform a Spin Attack?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Talk";
   M2Txt = "It can be used to launch enemies near you far away, or break blocks in your path.";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = "...But I also heard you could hold the button down while in the air so that you can glide down slower and farther...";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Happy";
   M4Txt = "...making jumping over difficult obstacles much easier, or even possible in the first place!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
