_X = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _X = _X - 0.8571428571428571;
      play();
   }
   else
   {
      stop();
   }
};
