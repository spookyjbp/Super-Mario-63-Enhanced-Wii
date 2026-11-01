onClipEvent(load){
   staramount = 0;
   frame = "idle";
   Mood = "Sad";
   M1Moo = "Sad";
   M1Txt = "..............................................................";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Sad";
   M2Txt = ".............................................................";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Happy";
   M3Txt = "Welcome to the Toad Fortress!!!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Talk";
   M4Txt = "It is one of the few Safe Haven\'s here in Lethal Lava Land ever since the Bully-Toad War started.";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
