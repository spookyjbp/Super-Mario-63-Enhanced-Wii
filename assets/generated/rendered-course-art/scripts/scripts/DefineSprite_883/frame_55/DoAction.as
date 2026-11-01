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
_root.Stats.screenfilter._alpha = 50;
