stop();
_root.warpframe = false;
hit = true;
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
   if(this.warp3.hitTest(_root.Course.Char.charClip.feet))
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.attackFrame = "none";
      warp = true;
      warpnum = 3;
   }
   if(warp == true && _root.warpframe == false)
   {
      if(warpnum == 1)
      {
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      else if(warpnum == 2)
      {
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      else if(warpnum == 3)
      {
         _root.attachMovie("lefttransition","lefttransition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:_root.screensizeX / 2,_y:_root.screensizeY / 2});
      }
      warp = false;
   }
   if(_root.warpframe == true)
   {
      if(warpnum == 1)
      {
         _root.changecourse("lefttransition2","3-4",2350,30,2350,30);
      }
      else if(warpnum == 2)
      {
         _root.changecourse("lefttransition2","3-2",2350,-70,2350,-70);
      }
      else if(warpnum == 3)
      {
         _root.changecourse("lefttransition2","3-8",0,0,0,0,true);
      }
   }
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char._y - _Y < 1600)
      {
         if(_root.Metal == false && _root.Invincible == false && _root.Invisible == false)
         {
            if(hit == false)
            {
               _root.HazymazeCaveSongChange(0);
            }
            hit = true;
         }
      }
      else if(_root.Metal == false && _root.Invincible == false && _root.Invisible == false)
      {
         if(hit == true)
         {
            _root.HazymazeCaveSongChange(1);
         }
         hit = false;
      }
   }
};
