onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
      if(go == true)
      {
         _root.Course.Char.yspeed = - _root.gravity;
         _root.Course.Char.xspeed = 0;
      }
   }
   else
   {
      stop();
   }
};
