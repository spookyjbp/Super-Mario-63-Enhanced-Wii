_root.Course.BackGFX.w1.play();
onEnterFrame = function()
{
   _root.Stats.screenfilter._alpha += 4;
   _root.Camcenter = _root.Course.BackGFX.w1;
   _root.PauseGame = false;
};
