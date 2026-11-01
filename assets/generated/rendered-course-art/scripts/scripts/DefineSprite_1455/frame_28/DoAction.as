onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Course.Char.xspeed *= 0.8;
      play();
   }
   else
   {
      stop();
   }
};
