onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.Course.Char.yspeed = Math.max(0,_root.Course.Char.yspeed);
};
_root.Maxzoomin = 200;
_root.CameraZoom = 100;
