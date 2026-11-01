onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Course.Char.yspeed -= _root.gravity / 2;
      play();
   }
   else
   {
      stop();
   }
};
