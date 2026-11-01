count = 0;
_root.Starnamenum = 20;
_root.AreaTextClipF("Boo\'s Mansion:\r<i>Secret of the Library</i>",3);
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
