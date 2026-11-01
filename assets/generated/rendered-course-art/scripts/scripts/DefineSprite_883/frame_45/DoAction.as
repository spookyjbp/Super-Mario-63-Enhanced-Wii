_root.Course.BackGFX.w1.gotoAndPlay(12);
_root.Course.BackGFX.w2.gotoAndPlay(12);
onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      _root.Stats.screenfilter._alpha += 5;
      _root.Camcenter = _parent._parent;
      play();
   }
};
