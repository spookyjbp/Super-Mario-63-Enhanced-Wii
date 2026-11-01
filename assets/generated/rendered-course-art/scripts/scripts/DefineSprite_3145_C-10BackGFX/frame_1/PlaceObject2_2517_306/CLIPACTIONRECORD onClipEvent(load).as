onClipEvent(load){
   staramount = 2;
   if(random(2) == 0)
   {
      this._y = 500;
   }
   _X = _X + random(20);
   Mood = "Sad";
   M1Moo = "Mad";
   M1Txt = ".... Ugghhh..... You know about the 2008 Economic Crash?";
   M1Sta = function()
   {
      return BP();
   };
   M1End = function()
   {
      return BP();
   };
   M2Moo = "Mad";
   M2Txt = "....Well, I had all my money placed in those stocks.....";
   M2Sta = function()
   {
      return true;
   };
   M2End = function()
   {
      return BP();
   };
   M3Moo = "Crazy";
   M3Txt = "And now thy\'re all gone!!!";
   M3Sta = function()
   {
      return true;
   };
   M3End = function()
   {
      return BP();
   };
   M4Moo = "Mad";
   M4Txt = ".......<i>Lousy Bush...</i>";
   M4Sta = function()
   {
      return true;
   };
   M4End = function()
   {
      return BP();
   };
   M6Moo = "Crazy";
   M6Txt = "I keep forgetting where the Secret Passage is once I find it!!";
   M6Sta = function()
   {
      return true;
   };
   M6End = function()
   {
      return BP();
   };
   M7Moo = "Sad";
   M7Txt = ".......................";
   M7Sta = function()
   {
      return true;
   };
   M7End = function()
   {
      return BP();
   };
   M8Moo = "Confused";
   M8Txt = "...Huh?.......Who are you?!";
   M8Sta = function()
   {
      return true;
   };
   M8End = function()
   {
      return BP();
   };
}
