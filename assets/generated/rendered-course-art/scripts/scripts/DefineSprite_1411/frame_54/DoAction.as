onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      stop();
      if(_root.Course.Char.air == false || _root.Water == true)
      {
         gotoAndStop("land");
         play();
         onEnterFrame = function()
         {
            if(_root.PauseGame == false)
            {
               play();
            }
            else
            {
               stop();
            }
         };
      }
   }
   else
   {
      stop();
   }
};
