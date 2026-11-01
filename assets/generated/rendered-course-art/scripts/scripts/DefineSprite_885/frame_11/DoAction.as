_root.Course.FrontGFX.gotoAndStop(1);
_root.playpause = true;
_root.StopBGsong();
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
