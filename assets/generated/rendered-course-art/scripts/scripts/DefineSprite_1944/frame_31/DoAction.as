onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.Course.Char._rotation *= 0.8;
      play();
   }
   else
   {
      stop();
   }
};
