stop();
onEnterFrame = function()
{
   _X = _X - 3;
   if(_X < -20)
   {
      _X = 500;
   }
   if(_X - _parent.mario._x < 5 && _X < 450 && (_root.preloader.mario._currentframe <= 22 || _root.preloader.mario._currentframe >= 26))
   {
      _root.TotalCoins = _root.TotalCoins + 1;
      play();
      onEnterFrame = null;
   }
};
