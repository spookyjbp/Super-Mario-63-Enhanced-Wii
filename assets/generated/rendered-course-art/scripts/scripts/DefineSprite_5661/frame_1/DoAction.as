onEnterFrame = function()
{
   _root.Camcenter = _parent._parent;
   if(_root.PauseGame == false)
   {
      _parent._parent.yspeed *= 0.8;
      play();
   }
   else
   {
      stop();
   }
};
_parent._parent.yspeed = 5;
_parent._parent.xspeed = 0;
