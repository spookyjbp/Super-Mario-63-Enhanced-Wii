onClipEvent(load){
   if(random(2) == 0 && _parent.toaddisp1 == undefined)
   {
      staramount = 8;
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
      _parent.toaddisp1 = true;
      this.swapDepths(0);
      this.removeMovieClip();
      this._y = 400;
   }
}
