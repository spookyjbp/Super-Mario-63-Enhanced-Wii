if(_root.Course.Char.hitwall !== true)
{
   _root.Course.Char.hitwall = false;
   gotoAndStop("afterwall");
   play();
}
else
{
   _root.Course.Char.hitwall = false;
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         xspeed = 0;
         yspeed = - _root.gravity;
         play();
      }
      else
      {
         stop();
      }
   };
}
