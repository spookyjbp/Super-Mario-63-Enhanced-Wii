onClipEvent(load){
   staramount = 0;
   this._xscale = -100;
   Mood = "Sad";
   M1Fra = "walk";
   M1Moo = "Happy";
   M1Txt = "YES!! Finally I see someone else in the Castle other than me!!";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Fra = "idle";
   M2Moo = "Crazy";
   M2Txt = "Ever since Bowser attacked the Castle, he scared away all the Toads.......";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = "...All, but me and a few Toads, ran away.";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Talk";
   M4Txt = "...Hopefully now that your starting to restore the Shine Sprites to the Castle, some of the Toads will come back.";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
