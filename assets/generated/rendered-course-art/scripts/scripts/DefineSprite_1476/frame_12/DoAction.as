onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Course.Char.yspeed = - _root.gravity;
      _root.Course.Char.xspeed = 0;
      play();
   }
   else
   {
      stop();
   }
};
