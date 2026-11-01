stop();
_root.AreaTextClipF("Inside the Castle:\r<i>Upper Hallway</i>",55);
playedsound = false;
_root.warpframe = false;
_root.Starnamenum = 0;
onEnterFrame = function()
{
   if(this.warp1.hitTest(_root.Course.Char.charClip.feet) && (_root.BowserKey2 == true || _root.CheatCastleUnlock == true))
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
   if(this.warp3.hitTest(_root.Course.Char.charClip.feet))
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.attackFrame = "none";
      warp = true;
      warpnum = 3;
   }
   if(this.warp4.hitTest(_root.Course.Char.charClip.feet))
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.attackFrame = "none";
      warp = true;
      warpnum = 4;
   }
   if(warp == true && _root.warpframe == false)
   {
      if(warpnum == 1)
      {
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      if(warpnum == 2)
      {
         _root.attachMovie("downtransition","downtransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      if(warpnum == 3)
      {
         if(playedsound == false)
         {
            _root.PaintingSound();
         }
         playedsound = true;
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      if(warpnum == 4)
      {
         if(playedsound == false)
         {
            _root.PaintingSound();
         }
         playedsound = true;
         _root.attachMovie("downtransition","downtransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.changecourse("lefttransition2","C-2",-150,0,270,-425);
      }
      if(warpnum == 2)
      {
         _root.changecourse("downtransition2","C-9",90,0,-90,-390);
      }
      if(warpnum == 3)
      {
         _root.Fluddpow = "";
         _root.Restartcoins();
         _root.StopBGsong();
         _root.changecourse("lefttransition2","9-10",0,-230,0,-230,true);
      }
      if(warpnum == 4)
      {
         _root.Fluddpow = "";
         _root.Restartcoins();
         _root.StopBGsong();
         _root.changecourse("downtransition2","9-03-2",-270,-990,-270,-990,true);
      }
   }
};
