if(followpath == true)
{
   stop();
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         _root.Course.Char.yspeed -= _root.gravity;
         if(followpath == true)
         {
            stop();
         }
         else
         {
            play();
         }
         followpath = false;
      }
      else
      {
         stop();
      }
   };
}
