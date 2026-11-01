onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      nextFrame();
      _root.Course.Char.yspeed *= 0.97;
      _parent.chargeframe = _currentframe;
   }
   else
   {
      stop();
   }
};
