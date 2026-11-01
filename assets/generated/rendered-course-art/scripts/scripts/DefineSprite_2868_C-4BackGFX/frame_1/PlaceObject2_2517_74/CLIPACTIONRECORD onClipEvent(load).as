onClipEvent(load){
   staramount = 0;
   _xscale = -100;
   Mood = "Smile";
   M1Moo = "Happy";
   if(random(2) == 0)
   {
      this._y = 500;
   }
   M1Txt = "......<i> La-di-da-di-da!</i>...";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Crazy";
   M2Txt = "...<b><i>AAAAG@@GGH$GGGH&AOMGAAA@HAA#AAA@AAROFLAHGHA!!!!!!!B^&^!!!!!!$$#@$!!!</b></i>";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = "............I got a paper cut...";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
}
