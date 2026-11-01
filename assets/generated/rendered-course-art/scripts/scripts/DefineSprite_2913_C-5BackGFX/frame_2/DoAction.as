stop();
onEnterFrame = function()
{
   if(this.box.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == false)
   {
      _root.Course.Platforms.trapdoor.play();
      _root.Course.BackGFX.trapdoorgfx.play();
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
         _root.attachMovie("downtransition","downtransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      if(warpnum == 2)
      {
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.Fluddpow = "";
         _root.StopBGsong();
         _root.Restartcoins();
         _root.changecourse("downtransition2","8-E2-1",0,0,0,0,true);
      }
      if(warpnum == 2)
      {
         _root.changecourse("lefttransition2","C-4-SC1",350,-170,350,-170);
      }
   }
};
