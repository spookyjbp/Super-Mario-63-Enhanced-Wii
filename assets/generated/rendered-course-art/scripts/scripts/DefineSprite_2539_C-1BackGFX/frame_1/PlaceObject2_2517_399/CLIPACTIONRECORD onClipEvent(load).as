onClipEvent(load){
   if(_root.TotalStars <= 0 && _root.Star[39] == false && _root.newstar !== true)
   {
      staramount = 0;
      Mood = "Smile";
      M1Moo = "Mad";
      M1Txt = "<b><i>Eddie: </i></b>.........It\'s so Dark in here ...... ";
      M1Sta = function()
      {
         if(count2++ > 5)
         {
            return true;
         }
         return false;
      };
      standnext = false;
      M1End = function()
      {
         return BP();
      };
      count2 = 0;
      M2Moo = "Happy";
      M2Txt = "<b><i>Eddie: </i></b> Oh well!  Just go right through this door!";
      M2Sta = function()
      {
         return true;
      };
      M2End = function()
      {
         return BP();
      };
      M2F = function()
      {
         M1Sta = function()
         {
            return false;
         };
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
      this._y = 400;
   }
}
