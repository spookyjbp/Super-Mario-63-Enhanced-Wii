onClipEvent(load){
   if(random(2) == 0 && (_parent.toaddisp8 == "top" || _parent.toaddisp8 == undefined))
   {
      _parent.toaddisp8 = "top";
      staramount = 12;
      Mood = "Smile";
      M1Moo = "Talk";
      M1Txt = "So you say you\'ve collected " + _root.TotalStars + " Shine Sprites already....";
      M1Sta = function()
      {
         return BP();
      };
      M1End = function()
      {
         return BP();
      };
      M2Moo = "Confused";
      M2Txt = "O RLY?";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M3Moo = "Shocked";
      M3Txt = ".....................NO WAI!!!";
      M3Sta = function()
      {
         return true;
      };
      M3End = function()
      {
         return BP();
      };
      M3F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay("yes");
      };
      M4Moo = "Happy";
      M4Txt = "....Wow!!!...............";
      M4Sta = function()
      {
         return true;
      };
      M4End = function()
      {
         return BP();
      };
      M4F = function()
      {
         _root.Course.Char.charClip.frame.gotoAndPlay(1);
      };
   }
   else
   {
      _parent.toaddisp8 = "bottom";
      this._y = 400;
   }
}
