onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      nextFrame();
      _root.Course.Char.yspeed *= 0.97;
      _parent._parent._parent.chargeframe = _currentframe;
   }
};
