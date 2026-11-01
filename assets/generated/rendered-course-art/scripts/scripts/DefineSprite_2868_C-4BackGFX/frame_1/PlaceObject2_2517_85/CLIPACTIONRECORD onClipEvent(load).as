onClipEvent(load){
   staramount = 0;
   if(random(2) == 0)
   {
      this._y = 500;
   }
   Mood = "Smile";
   M1Moo = "Shocked";
   M1Txt = ".....I don\'t remember a hallway being here before!";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
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
