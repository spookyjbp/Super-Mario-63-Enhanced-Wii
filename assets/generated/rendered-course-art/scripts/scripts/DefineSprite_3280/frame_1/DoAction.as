onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
      if(_parent._parent.exploding == true)
      {
         nextFrame();
         play();
      }
   }
   else
   {
      stop();
   }
};
