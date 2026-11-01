onClipEvent(load){
   staramount = 0;
   Mood = "Smile";
   M1Moo = "Talk";
   M1Txt = "...Hello " + _root.CurrentPlayer + ", How are you?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Confused";
   M2Txt = "...What... What you say??";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Shocked";
   M3Txt = "...Huh?  You mean we are both standing 100\'s of feet off the ground on a floating cloud?!?!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Shocked";
   M4Txt = "..........................................How did I end up here?!?!?!";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M5Moo = undefined;
   M5Txt = undefined;
   M5Sta = function()
   {
      return true;
   };
   M5End = function()
   {
      return BP();
   };
   M6Moo = "Talk";
   M6Txt = "But none the less, I am still enjoying that game thoroughly, so I have no other complaints...";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
}
