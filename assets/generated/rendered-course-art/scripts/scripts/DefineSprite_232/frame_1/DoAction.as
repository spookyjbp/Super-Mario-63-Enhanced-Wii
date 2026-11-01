onEnterFrame = function()
{
   frame = 1;
   if(_root.RedCoins <= 0)
   {
      frame = 2;
   }
   if(_root.SilverStars <= 0)
   {
      frame = 3;
   }
   if(_root.RedCoins <= 0 && _root.SilverStars <= 0)
   {
      frame = 4;
   }
   gotoAndStop(frame);
};
