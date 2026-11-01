onEnterFrame = function()
{
   OEF();
};
OEF = function()
{
   _root.PauseGame = false;
   _root.Camcenter = _root.Course.BackGFX.box1;
};
_root.Course.BackGFX.nextFrame();
