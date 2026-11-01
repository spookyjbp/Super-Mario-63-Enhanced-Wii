onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char.attackFrame == "Star")
      {
         stop();
      }
      else
      {
         play();
      }
   }
   else
   {
      stop();
   }
};
gotoAndPlay(_currentframe + random(40));
