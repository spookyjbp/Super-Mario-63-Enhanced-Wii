onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      _root.Camcenter = _parent._parent;
      _parent._parent._alpha -= 5;
      play();
   }
};
