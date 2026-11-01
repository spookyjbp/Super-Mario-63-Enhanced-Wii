onClipEvent(load){
   staramount = 10;
   _X = _X - random(50);
   Mood = "Sad";
   M1Moo = "Talk";
   M1Txt = "It\'s kind of sad...";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Talk";
   M2Txt = "That Toad over there near the Tall Tall Mountain Painting...";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Talk";
   M3Txt = "...keeps on pretending that he\'s James Bond!!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = "He <i>\'Claims\'</i> that there is a secret passage that leads to this room.";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Talk";
   M5Txt = "<b>Nonsense!</b>";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
   M6Moo = "Talk";
   M6Txt = "There\'s no such thing as a Secret Passage in Princess Peach\'s Castle!";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
}
