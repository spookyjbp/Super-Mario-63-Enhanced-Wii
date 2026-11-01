onEnterFrame = function()
{
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
