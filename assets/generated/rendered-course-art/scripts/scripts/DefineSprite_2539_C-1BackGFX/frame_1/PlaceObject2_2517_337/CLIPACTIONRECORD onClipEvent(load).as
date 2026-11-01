onClipEvent(load){
   if(random(2) == 0 && (_parent.toaddisp2 == "top" || _parent.toaddisp2 == undefined))
   {
      _parent.toaddisp2 = "top";
      staramount = 4;
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "Whenever you get hurt by an attack, your HP is displayed on the top of the screen in a Pie shape.";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Talk";
      M2Txt = "Above it, it says POWER....";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Talk";
      M3Txt = "...Did you know that when you collect a Yellow Coin, one of the letters on POWER glows...";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M4Moo = "Happy";
      M4Txt = "...And when all 5 letters glow, you <b>heal 1 HP!!!</b>";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
   }
   else
   {
      _parent.toaddisp2 = "bottom";
      this._y = 400;
   }
}
