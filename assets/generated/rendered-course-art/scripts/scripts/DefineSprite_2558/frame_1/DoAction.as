onEnterFrame = function()
{
   if(_root.BowserKey2 == true || _root.CheatCastleUnlock == true)
   {
      gotoAndStop(8);
   }
   else if(_root.BowserKey1 == true)
   {
      gotoAndStop(7);
   }
   else if(_root.TotalStars >= 8)
   {
      gotoAndStop(6);
   }
   else if(_root.TotalStars >= 5)
   {
      gotoAndStop(5);
   }
   else if(_root.TotalStars >= 3)
   {
      gotoAndStop(4);
   }
   else if(_root.TotalStars >= 2)
   {
      gotoAndStop(3);
   }
   else if(_root.TotalStars == 1)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
