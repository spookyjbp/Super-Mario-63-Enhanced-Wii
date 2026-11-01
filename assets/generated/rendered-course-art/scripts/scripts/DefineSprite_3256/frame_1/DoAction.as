onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      play();
      if(_parent._parent._parent.exploding == true)
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
