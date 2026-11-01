heightlevel = 0;
_root.AreaTextClipF("Inside the Castle:\r<i>Main Hall</i>",1);
count = 0;
_root.warpframe = false;
_root.Starnamenum = 0;
onEnterFrame = function()
{
   if(_root.Course.Char._y - _Y > -34)
   {
      heightlevel = 0;
   }
   if((_root.Course.Char._y - _Y < -57 || _root.Course.Char._y - _Y < -30 && _root.Course.Char.air == false) && heightlevel < 1)
   {
      heightlevel = 1;
   }
   if((_root.Course.Char._y - _Y < -82 || _root.Course.Char._y - _Y < -55 && _root.Course.Char.air == false) && heightlevel < 2)
   {
      heightlevel = 2;
   }
   if(heightlevel == 2 && _root.Course.Char._y - _Y > -50)
   {
      heightlevel = 1;
   }
   gotoAndStop(heightlevel + 2);
   _root.Stats.screenfilter._alpha = Math.round((64 - _root.TotalStars) * 0.25);
   if(count++ > 4)
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
            _root.attachMovie("uptransition","uptransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
         }
      }
      if(_root.warpframe == true)
      {
         if(warpnum == 1)
         {
            _root.changecourse("righttransition2","C-4H",160,-170,160,-170);
         }
         if(warpnum == 2)
         {
            _root.changecourse("uptransition2","C-13",950,-50,950,-50);
         }
      }
   }
};
