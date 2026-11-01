onClipEvent(load){
   staramount = 3;
   if(random(3) < 1)
   {
      this._y = 500;
   }
   Mood = "Confused";
   M1Moo = "Talk";
   M1Txt = "You know....";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Talk";
   M2Txt = "I wonder why Princess Peach even ordered this slide to be built in the first place.";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = "It serves no realistic purpose, since it\'s 1-way.";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = "If it was a Doorway, or just even a Hallway, it would at least make sense.";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
}
