onClipEvent(load){
   this._xscale = -100;
   if(random(2) == 0)
   {
      this._y = 500;
   }
   staramount = 13;
   Mood = "Smile";
   M1Moo = "Happy";
   M1Txt = "I just got a copy of Super Smash Bros. Brawl yesterday!";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = "....Why am I not a playable character!?!?!";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = "I mean, well.......";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Crazy";
   M4Txt = " It\'s just a perfect idea!!!!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Talk";
   M5Txt = "My B-down would be \"Mushroom\" where I grow large and strong for a few seconds";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
   M6Moo = "Happy";
   M6Txt = "My B-Side would be.... um..... \"Kart!\"  Similar to that fat Wario guy\'s bike move thingy.";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
   M7Moo = "Talk";
   M7Txt = "My B-Up would be........ Hmmmmm...... I don\'t know.... well,";
   M7Sta = function()
   {
      return true;
   };
   M7End = function()
   {
      return BP();
   };
   M8Moo = "Mad";
   M8Txt = "And My B-neutral would be \"Princess Peach\" where I pull out Princess Peach and uses her as a Shield to oncoming attacks!";
   M8Sta = function()
   {
      return true;
   };
   M8End = function()
   {
      return BP();
   };
   M9Moo = "Crazy";
   M9Txt = "Captain Falcon: \"FALCON PAAAUUUWWNCH!!!!\"\rMe: \"Go! Princess Peach Shield!!\"";
   M9Sta = function()
   {
      return true;
   };
   M9End = function()
   {
      return BP();
   };
   M10Moo = "Crazy";
   M10Txt = "Peach: \"NO!! NOT A FALCON PUNCH!!!   AAAAAAAAAHHHHH!!!!!11!!!!111!oneoneone!\"";
   M10Sta = function()
   {
      return true;
   };
   M10End = function()
   {
      return BP();
   };
}
