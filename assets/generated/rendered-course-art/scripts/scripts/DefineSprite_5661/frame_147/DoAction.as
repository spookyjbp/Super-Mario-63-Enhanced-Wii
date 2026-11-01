_root.Course.BackGFX.nextFrame();
_root.Course.FrontGFX.nextFrame();
_root.Course.Platforms.nextFrame();
onEnterFrame = function()
{
   OEF();
};
OEF = function()
{
   _root.PauseGame = false;
   _root.Camcenter = _root.Course.BackGFX.box2;
};
