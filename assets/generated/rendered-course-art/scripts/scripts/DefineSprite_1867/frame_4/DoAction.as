stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char.air == false || _root.Course.Char.Water == true)
      {
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
   stop();
};
