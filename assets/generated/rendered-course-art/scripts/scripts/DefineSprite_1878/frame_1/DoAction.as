if(_root.Course.Char.landtime == 1)
{
   gotoAndStop("jump");
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
else
{
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
      if(_root.Course.Char.landtime == 2)
      {
         gotoAndStop("midjump");
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
   };
}
