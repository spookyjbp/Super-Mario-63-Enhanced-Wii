count = 0;
_root.Starnamenum = 15;
_root.AreaTextClipF("Hzy Maze Cave:\r<i>Secret of the Lake</i>",50);
stop();
onEnterFrame = function()
{
   if(count < 48)
   {
      _root.PauseGame = false;
      _root.Camspeed = 2;
      _root.Camcenter = star;
      _root.playpause = true;
      count++;
   }
   else if(count < 150)
   {
      _root.PauseGame = false;
      count++;
      _root.playpause = true;
      _root.MaxCamspeed = 35;
      _root.Camspeed = 30;
   }
   else
   {
      _root.playpause = false;
      nextFrame();
      _root.warpframe = false;
      onEnterFrame = null;
   }
};
