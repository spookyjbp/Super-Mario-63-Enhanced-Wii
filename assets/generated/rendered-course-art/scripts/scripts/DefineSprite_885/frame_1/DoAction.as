onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      play();
   }
};
