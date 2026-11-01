stop();
_root.AreaTextClipF("Mushroom Kingdom\r<i>(Tutorial)</i>",45);
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
   if(this.warp2.hitTest(_root.Course.Char.charClip.feet))
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.attackFrame = "none";
      warp = true;
      warpnum = 2;
   }
   if(warp == true && _root.warpframe == false)
   {
      if(warpnum == 1)
      {
         _root.attachMovie("righttransition","righttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      if(warpnum == 2)
      {
         _root.attachMovie("righttransition","righttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      warp = false;
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.changecourse("righttransition2","8-E1-1-2",0,0,0,0);
      }
      if(warpnum == 2)
      {
         _root.changecourse("righttransition2","8-E2-1",0,0,0,0);
      }
   }
};
