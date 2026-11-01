onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char.hitwall == true)
      {
         xspeed = 0;
         yspeed = - _root.gravity;
      }
      play();
   }
   else
   {
      stop();
   }
};
