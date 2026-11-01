stop();
_root.AreaTextClipF("Top of the Castle:\r<i>Star Room</i>",1);
playedsound = false;
_root.Starnamenum = 0;
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
         if(playedsound == false)
         {
            _root.PaintingSound();
         }
         playedsound = true;
         _root.attachMovie("uptransition","uptransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.Fluddpow = "";
         _root.StopBGsong();
         _root.Restartcoins();
         _root.changecourse("uptransition2","9-11",-560,-160,-560,-160,true);
      }
   }
};
