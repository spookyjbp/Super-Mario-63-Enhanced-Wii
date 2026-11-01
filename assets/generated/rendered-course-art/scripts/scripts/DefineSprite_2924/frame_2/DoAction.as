onEnterFrame = function()
{
   if(_root.PauseGame == true || _root.playpause == true)
   {
      stop();
   }
   else
   {
      play();
   }
};
