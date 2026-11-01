_root.playpause = true;
onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      _root.Camcenter = _parent._parent;
      play();
   }
};
