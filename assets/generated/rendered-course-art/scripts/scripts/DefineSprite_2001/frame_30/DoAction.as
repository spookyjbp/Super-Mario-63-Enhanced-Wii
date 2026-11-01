count = 0;
onEnterFrame = function()
{
   count += 0.5;
   count = Math.min(count,7);
   _root.PauseGame = false;
   _root.Course.Char.yspeed = Math.max(0,_root.Course.Char.yspeed);
   _root.CameraZoom += count;
   _root.Maxzoomin = 200;
};
