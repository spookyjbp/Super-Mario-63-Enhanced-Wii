onClipEvent(load){
   staramount = 15;
   if(_parent.toaddisp20 == undefined)
   {
      _parent.toaddisp20 = random(2);
   }
   if(_parent.toaddisp20 == 1)
   {
      Mood = "Smile";
      M1Moo = "Sad";
      M1Txt = "...............................";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Sad";
      M2Txt = ".................................";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Sad";
      M3Txt = "....................................";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Fra = "walk";
      M4Moo = "Happy";
      M4Txt = "<b><i>Never gonna    Give    you   up!  </b></i>";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M5Moo = "Happy";
      M5Txt = "<b><i>Never gonna    Let   you   down!    </b></i>";
      M5Sta = function()
      {
         return true;
      };
      M5End = function()
      {
         return BP();
      };
      M6Fra = "idle";
      M6Moo = "Sad";
      M6Txt = "...........................................................................Sorry.....";
      M6Sta = function()
      {
         return true;
      };
      M6End = function()
      {
         return BP();
      };
   }
   else
   {
      this._y = 500;
   }
}
