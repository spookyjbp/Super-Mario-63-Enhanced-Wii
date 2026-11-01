onClipEvent(load){
   _xscale = -100;
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = "<i>(Shouting):</i>  <b>Oh... Why Hello There!</b>";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Confused";
   M2Txt = "<b>.....WHAT???... What did you say?!?!</b>";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Mad";
   M3Txt = "<b>I can\'t here a word you are saying over the roar of the Lava Wheel!!</b>";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Crazy";
   M4Txt = "..........................";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = "Happy";
   M5Txt = "Just Kidding!!!!";
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
}
