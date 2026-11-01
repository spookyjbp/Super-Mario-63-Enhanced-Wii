onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
_root.Course.Char.yspeed -= 10;
_root.Course.Char.xspeed += _parent._xscale / 50;
