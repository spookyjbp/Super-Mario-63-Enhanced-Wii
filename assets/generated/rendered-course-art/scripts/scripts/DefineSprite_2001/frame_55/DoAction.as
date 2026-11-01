onEnterFrame = function()
{
   count -= 0.5;
   count = Math.max(count,0);
   _root.PauseGame = false;
   _root.Course.Char.yspeed = Math.max(0,_root.Course.Char.yspeed);
   _root.CameraZoom += count;
   _root.Maxzoomin = 200;
};
