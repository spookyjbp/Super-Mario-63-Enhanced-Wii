stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 1000 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 1000 && frame !== "gone")
      {
         if(frame == "flying")
         {
            xspeed = flyxspeed;
            yspeed = flyyspeed;
            if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.yspeed > 0)
            {
               _root.Course.Char.yspeed /= 2.5;
               _root.Course.Char.yspeed -= 12;
               i = 0;
               while(i < 8)
               {
                  _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y});
                  i++;
               }
               frame = "run";
               blink = 10;
               _root.bgsoundFX = new Sound(this);
               _root.bgsoundFX.attachSound("flippers");
               _root.bgsoundFX.start(0,1);
               _root.bgsoundFX.setVolume(_root.BgVolume);
            }
            if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water))
            {
               i = 0;
               while(i < 8)
               {
                  _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y});
                  i++;
               }
               frame = "run";
               blink = 10;
               yspeed = -4;
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  xspeed = 4;
               }
               else
               {
                  xspeed = -4;
               }
            }
            if(this.body.hitTest(_root.Course.Char.charClip.body))
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(1,90,-3,5,7,"");
               }
               else
               {
                  _root.MarioHit(1,90,-3,-5,7,"");
               }
            }
            if(frame == "flying" || frame == "run")
            {
               if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  yspeed = -4;
                  xspeed = 0;
                  i = 0;
                  while(i < 8)
                  {
                     _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y});
                     i++;
                  }
                  frame = "run";
                  blink = 10;
               }
               else
               {
                  if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                  {
                     yspeed = -4;
                     xspeed = 4;
                     i = 0;
                     while(i < 8)
                     {
                        _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y});
                        i++;
                     }
                     frame = "run";
                     blink = 10;
                  }
                  if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                  {
                     yspeed = -4;
                     xspeed = -4;
                     i = 0;
                     while(i < 8)
                     {
                        _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y});
                        i++;
                     }
                     frame = "run";
                     blink = 10;
                  }
               }
            }
         }
         else
         {
            if(blink > 0)
            {
               blink--;
            }
            if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               air = false;
               yspeed = 0;
               if(blink >= 0)
               {
                  xspeed = runspeed;
               }
            }
            else
            {
               air = true;
               if(blink >= 0)
               {
                  yspeed += _root.gravity;
               }
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               air = false;
               _Y--;
            }
            while(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _X--;
               _xscale = 100;
            }
            while(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _X++;
               _xscale = -100;
            }
            while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
            {
               _Y++;
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y--;
            }
            if(blink <= 0)
            {
               if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water))
               {
                  i = 0;
                  while(i < 8)
                  {
                     _root.Course.BackGFX.attachMovie("SkeletonGoonieBone","SkeletonGoonieBone" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y});
                     i++;
                  }
                  frame = "run";
               }
               if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
               {
                  _root.Course.Char.yspeed /= 2.5;
                  _root.Course.Char.yspeed -= 9;
                  frame = "gone";
                  _root.bgsoundFX = new Sound(this);
                  _root.bgsoundFX.attachSound("flippers");
                  _root.bgsoundFX.start(0,1);
                  _root.bgsoundFX.setVolume(_root.BgVolume);
               }
               if(this.body.hitTest(_root.Course.Char.charClip.body))
               {
                  blink = 7;
                  if(_root.Course.Char._x > _X + _parent._x)
                  {
                     _root.MarioHit(1,90,-2,4,7,"");
                  }
                  else
                  {
                     _root.MarioHit(1,90,-2,-4,7,"");
                  }
               }
               if(frame == "flying" || frame == "run")
               {
                  if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                  {
                     frame = "gone";
                  }
                  else
                  {
                     if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                     {
                        frame = "gone";
                     }
                     if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                     {
                        frame = "gone";
                     }
                  }
               }
            }
         }
         if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            frame = "gone";
         }
         bounds = _root.Course.CamEdge.getBounds(_root.Course.CamEdge);
         coursewidth = bounds.xMax - bounds.xMin;
         courseheight = bounds.yMax - bounds.yMin;
         if(_X > bounds.xMax + 5)
         {
            _X = _X - coursewidth;
         }
         if(_X < bounds.xMin - 5)
         {
            _X = _X + coursewidth;
         }
         if(_Y > bounds.yMax + 5)
         {
            _Y = _Y - courseheight;
         }
         if(_Y < bounds.yMin - 5)
         {
            _Y = _Y + courseheight;
         }
         _X = _X + xspeed * (- _xscale) / 100;
         _Y = _Y + yspeed;
         clip.gotoAndStop(frame);
      }
      else if((Math.abs(_root.currentCamX - (_X + _parent._x)) > 1000 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 1000) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 1000 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 1000))
      {
         restartvars();
      }
   }
};
