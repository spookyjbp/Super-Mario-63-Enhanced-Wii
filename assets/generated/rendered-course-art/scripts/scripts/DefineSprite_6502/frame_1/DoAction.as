onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
