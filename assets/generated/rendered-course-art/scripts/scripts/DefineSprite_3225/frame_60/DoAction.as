_root.Maxzoomin = 250;
onEnterFrame = function()
{
   _root.CameraZoom += 3;
   _root.CameraZoom = Math.min(250,_root.CameraZoom);
   _root.Camcenter = _root.Course.BackGFX.target1;
   _root.PauseGame = false;
};
