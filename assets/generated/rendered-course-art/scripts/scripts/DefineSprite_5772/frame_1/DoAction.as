onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _parent._parent._x -= _parent._parent._xscale / 100 * 2;
      play();
   }
   else
   {
      stop();
   }
};
