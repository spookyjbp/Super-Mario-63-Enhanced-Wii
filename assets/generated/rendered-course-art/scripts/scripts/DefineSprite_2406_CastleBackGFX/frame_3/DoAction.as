stop();
_root.warpframe = false;
bnum = random(30) + 30;
bcount = 0;
btype = random(6) + 1;
this.createEmptyMovieClip("birdsound",_root.getNextHighestDepth());
onEnterFrame = function()
{
   color += Math.round(_root.TotalStars * 1.5 + 25 - color) / 7;
   if(_root.CheatCastleRestore == true)
   {
      color = 100;
   }
   if(color > 100)
   {
      oldcolor = color;
      color += Math.abs(100 - color) * 2.5;
   }
   color2 = Math.abs(100 - color);
   colorTrans = {ra:color * 0.5 + 50 - color2 / 3,rb:0,ga:color * 0.5 + 50 - color2 / 3,gb:0,ba:color * 0.5 + 50 - color2 / 3,bb:0,aa:100,ab:0};
   myColor.setTransform(colorTrans);
   if(color > 100)
   {
      color = oldcolor;
   }
   if(_root.TotalStars > 20)
   {
      birdvolume = 1;
      bcount++;
      if(bcount > bnum)
      {
         birdsound = new Sound(this);
         birdsound.attachSound("Birdy" + btype);
         birdsound.start(0,1);
         birdsound.setVolume(Math.round(_root.MarioVolume / 2) * birdvolume);
         bcount = 0;
         btype = random(6) + 1;
         bnum = random(50) + (73 - _root.TotalStars);
      }
   }
   if(this.warp1.hitTest(_root.Course.Char.charClip.feet) && _root.Star[39] == true)
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
         _root.changecourse("lefttransition2","8-E1-2-2",964,-783,964,-783,true);
      }
      if(warpnum == 2)
      {
         _root.Fluddpow = "";
         _root.Restartcoins();
         _root.changecourse("righttransition2","K-1",0,0,0,0,true);
      }
   }
};
