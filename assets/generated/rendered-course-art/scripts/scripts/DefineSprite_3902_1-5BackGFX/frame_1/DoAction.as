count = 0;
stop();
_root.AreaTextClipF("Bob-Omb Battlefield:\r<i>Secret of the Mountain</i>",60);
onEnterFrame = function()
{
   _root.PauseGame = false;
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
      onEnterFrame = function()
      {
         if(this.warp1.hitTest(_root.Course.Char.charClip.feet))
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.yspeed = 0;
            _root.Course.Char.attackFrame = "none";
            warp = true;
            warpnum = 1;
         }
         if(warp == true && _root.warpframe == false)
         {
            if(warpnum == 1)
            {
               _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
            }
            warp = false;
         }
         if(_root.warpframe == true)
         {
            if(warpnum == 1)
            {
               _root.changecourse("lefttransition2","1-4",1950,-650,1950,-650,true);
            }
         }
      };
   }
};
