onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _parent.xspeed = 0;
      play();
   }
   else
   {
      stop();
   }
};
