onClipEvent(load){
   staramount = 0;
   _X = _X + (random(180) + random(2) * 330);
   count2 = 0;
   OEF = function()
   {
      count2++;
      if(count2 > 32)
      {
         count2 = 0;
         _xscale = _xscale * -1;
      }
   };
   Mood = "Sad";
   M1Moo = "Talk";
   M1Txt = "...I heard that there\'s a hidden door somewhere here.";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Talk";
   M2Txt = "It\'s a door that almost perfectly camouflages itself against a wall.";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = "All you can see is a small outline for the doorframe...";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = "I\'m absolutely sure it\'s in this hallway;";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Crazy";
   M5Txt = "But I can\'t seem to find it!";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
}
