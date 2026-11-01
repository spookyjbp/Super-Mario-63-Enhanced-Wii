onClipEvent(load){
   if(random(2) == 0 && (_parent.toaddisp2 == "bottom" || _parent.toaddisp2 == undefined))
   {
      _parent.toaddisp2 = "bottom";
      staramount = 4;
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "Hello, " + _root.CurrentPlayer;
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Talk";
      M2Txt = "Did you know that there is a secret passage near the Snowman\'s Land painting?";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
   }
   else
   {
      _parent.toaddisp2 = "top";
      this._y = 400;
   }
}
