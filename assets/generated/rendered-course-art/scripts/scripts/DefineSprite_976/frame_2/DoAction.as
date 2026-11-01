_root.bgsong.stop();
saveY = _root.Course.Char.yspeed;
saveX = _root.Course.Char.xspeed;
onEnterFrame = function()
{
   _root.Camcenter = _parent;
   _root.playpause = true;
   _root.PauseGame = false;
};
