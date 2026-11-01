stop();
onEnterFrame = function()
{
   _root.Course.FrontGFX._alpha -= 5;
   _root.Course.BackGFX._alpha -= 5;
   _root.Course.BackGFX._alpha = Math.max(_root.Course.BackGFX._alpha,0);
   _root.Course.FrontGFX._alpha = Math.max(_root.Course.FrontGFX._alpha,0);
   _root.CameraZoom = 250;
   _root.Camcenter = _root.Course.BackGFX.target1;
   _root.PauseGame = false;
};
