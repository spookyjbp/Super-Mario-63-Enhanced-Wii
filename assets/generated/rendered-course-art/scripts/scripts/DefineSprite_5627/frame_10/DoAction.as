onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _parent._parent.yspeed--;
      _parent._parent.yspeed *= 0.8;
      _parent._parent.targety = -200;
      play();
   }
   else
   {
      stop();
   }
};
