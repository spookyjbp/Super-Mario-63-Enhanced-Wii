stop();
_root.Starnamenum = 42;
_root.Course.BackGFX.StartClip._y = 40;
_root.Course.BackGFX.StartClip._x = 1000;
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
         _root.attachMovie("uptransition","uptransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      if(warpnum == 2)
      {
         _root.attachMovie("downtransition","downtransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      warp = false;
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.changecourse("uptransition2","8-E5-3",-1500,180,-1500,180);
      }
      if(warpnum == 2)
      {
         _root.changecourse("downtransition2","8-E5-1",375,-650,375,-650);
      }
   }
};
