stop();
bnum = random(30) + 30;
bcount = 0;
_root.warpframe = false;
onEnterFrame = function()
{
   bcount++;
   if(bcount > bnum)
   {
      _root.Mariosound = new Sound(this);
      btype = random(6) + 1;
      _root.Mariosound.attachSound("Birdy" + btype);
      _root.Mariosound.start(0,1);
      _root.Mariosound.setVolume(Math.round(_root.MarioVolume / 1.5));
      bcount = 0;
      bnum = random(50) + 10;
   }
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
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
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
         _root.RestartFludd();
         _root.BossCheckpoint = false;
         _root.changecourse("lefttransition2","9-07",0,0,0,0,true);
      }
      if(warpnum == 2)
      {
      }
   }
};
