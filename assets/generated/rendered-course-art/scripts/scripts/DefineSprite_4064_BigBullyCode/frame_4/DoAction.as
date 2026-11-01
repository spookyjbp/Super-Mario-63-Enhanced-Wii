stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(frame !== "dead")
      {
         stun--;
         stun = Math.max(stun,0);
         if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 500 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
         {
            _yscale = scale * 50;
            if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               if(air == true && Math.abs(yspeed) > 3)
               {
                  yspeed = Math.max((- yspeed) * 0.7,yspeed * 0.7);
               }
               else if(air == true)
               {
                  yspeed = 0;
               }
               yspeed = Math.min(yspeed,0);
               air = false;
            }
            else
            {
               air = true;
            }
            while(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _X--;
               xspeed = Math.max(xspeed * 0.7,(- xspeed) * 0.7);
            }
            while(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _X++;
               xspeed = Math.min(xspeed * 0.7,(- xspeed) * 0.7);
            }
            while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
            {
               _Y++;
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y--;
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y--;
            }
            if(air == true)
            {
               yspeed += _root.gravity;
               yspeed = Math.min(yspeed,7);
            }
            if(air == false && frame !== "hurt")
            {
               if(this.eye.hitTest(_root.Course.Char.charClip.body) && stun == 0)
               {
                  seeing = true;
                  if(Math.abs(_root.Course.Char._x - (_X + _parent._x)) < 300)
                  {
                     if(_root.Course.Char._x > _X + _parent._x)
                     {
                        _xscale = scale * 50;
                        if(Moveright())
                        {
                           xspeed -= Math.min((xspeed - 7) / 13,0) * scale;
                        }
                     }
                     else
                     {
                        _xscale = (- scale) * 50;
                        if(Moveleft())
                        {
                           xspeed -= Math.max((xspeed + 7) / 13,0) * scale;
                        }
                     }
                  }
               }
               else
               {
                  seeing = false;
               }
            }
            if(air == false)
            {
               xspeed = _root.groundFriction(xspeed,0.1,1.05);
            }
            if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 0.6 / scale;
               xspeed += (Math.random() * 2 - 1 - xspeed / 1.5) / scale;
               _Y = _Y - 3;
            }
            else
            {
               if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  yspeed -= 0.6 / scale;
                  xspeed += Math.max((6 - xspeed / 1.5) / scale,0);
                  _Y = _Y - 3;
               }
               if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  yspeed -= 0.6 / scale;
                  xspeed += Math.min((-6 - xspeed / 1.5) / scale,0);
                  _Y = _Y - 3;
               }
            }
            if(this.body.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 5)
            {
               frame = "hurt";
               stun = 15;
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  mxspeed = xspeed;
                  mxspeed = Math.max(mxspeed,0);
                  _root.Course.Char.xspeed *= 0.5;
                  _root.Course.Char.xspeed += mxspeed * 0.3 + 3;
               }
               else
               {
                  mxspeed = xspeed;
                  mxspeed = Math.min(mxspeed,0);
                  _root.Course.Char.xspeed *= 0.5;
                  _root.Course.Char.xspeed += mxspeed * 0.3 - 3;
               }
               yspeed -= Math.abs((_root.Course.Char.xspeed / 2 + 1) / 7) / scale;
               xspeed += (_root.Course.Char.xspeed / 1.5 - xspeed / 1.5) * 5 / scale;
               _Y = _Y - 3;
            }
            if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water) || this.head.hitTest(_root.Course.Char.charClip.water) || this.head.hitTest(_root.Course.Char.charClip.frame.water))
            {
               frame = "hurt";
               stun = 15;
               yspeed -= 1.5 / scale;
               if(_root.Course.Char.attackFrame == "Spin" && _root.Course.Char.attack == true)
               {
                  if(_root.Course.Char._x > _X + _parent._x)
                  {
                     mxspeed = xspeed;
                     _root.Course.Char.xspeed *= 0.7;
                     _root.Course.Char.xspeed += mxspeed * 0.3 + 2.5;
                  }
                  else
                  {
                     mxspeed = xspeed;
                     _root.Course.Char.xspeed *= 0.7;
                     _root.Course.Char.xspeed += mxspeed * 0.3 - 2.5;
                  }
                  if(_root.Course.Char._x > _X + _parent._x)
                  {
                     xspeed *= 0.7;
                     xspeed -= 3 / scale;
                     stun = 15;
                  }
                  else
                  {
                     xspeed *= 0.7;
                     xspeed += 3 / scale;
                     stun = 12;
                  }
               }
               else if(_root.Course.Char._xscale > 0)
               {
                  if(_root.Fluddpow == "T" || _root.Fluddpow == "H" && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true)
                  {
                     xspeed *= 0.9;
                     xspeed += 2 / scale;
                     xspeed += _root.Course.Char.xspeed / 2;
                     stun = 7;
                     if(_root.Course.Char._x > _X + _parent._x)
                     {
                        mxspeed = xspeed;
                        mxspeed = Math.max(mxspeed,0);
                        _root.Course.Char.xspeed *= 0.3;
                        _root.Course.Char.xspeed += mxspeed * 0.3 + 3;
                     }
                     else
                     {
                        mxspeed = xspeed;
                        mxspeed = Math.min(mxspeed,0);
                        _root.Course.Char.xspeed *= 0.3;
                        _root.Course.Char.xspeed += mxspeed * 0.3 - 3;
                     }
                  }
                  else
                  {
                     xspeed *= 0.9;
                     xspeed -= 2 / scale;
                     stun = 15;
                  }
               }
               else if(_root.Fluddpow == "T" || _root.Fluddpow == "H" && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true)
               {
                  xspeed *= 0.9;
                  xspeed -= 2 / scale;
                  xspeed += _root.Course.Char.xspeed / 2;
                  stun = 7;
                  if(_root.Course.Char._x > _X + _parent._x)
                  {
                     mxspeed = xspeed;
                     mxspeed = Math.max(mxspeed,0);
                     _root.Course.Char.xspeed *= 0.3;
                     _root.Course.Char.xspeed += mxspeed * 0.3 + 3;
                  }
                  else
                  {
                     mxspeed = xspeed;
                     mxspeed = Math.min(mxspeed,0);
                     _root.Course.Char.xspeed *= 0.3;
                     _root.Course.Char.xspeed += mxspeed * 0.3 - 3;
                  }
               }
               else
               {
                  xspeed *= 0.9;
                  xspeed += 2 / scale;
                  stun = 15;
               }
               _Y = _Y - 3;
            }
            if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.yspeed > 1 && _root.Course.Char.attack == false)
            {
               MakeSound();
               stun = 15;
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  mxspeed = xspeed;
                  xspeed += (- _root.Course.Char.yspeed) / 1.5 - 2 + _root.Course.Char.xspeed / 2;
                  _root.Course.Char.yspeed /= 2;
                  _root.Course.Char.yspeed -= 9 + Math.abs(mxspeed) / 2;
                  _root.Course.Char.xspeed *= -0.5;
                  _root.Course.Char.xspeed += mxspeed * 0.7;
               }
               else
               {
                  mxspeed = xspeed;
                  xspeed += _root.Course.Char.yspeed / 1.5 + 2 + _root.Course.Char.xspeed / 2;
                  _root.Course.Char.yspeed /= 2;
                  _root.Course.Char.yspeed -= 9 + Math.abs(mxspeed) / 2;
                  _root.Course.Char.xspeed *= -0.5;
                  _root.Course.Char.xspeed += mxspeed * 0.7;
               }
            }
            if(stun <= 0 && frame !== "hurt" && this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attack == false || !(_root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 5)) && !(Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T"))
            {
               stun = 3;
               MakeSound();
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  mxspeed = _root.Course.Char.xspeed;
                  _root.MarioHit(0,0,5,xspeed + 3 + xspeed / 2,13,"none");
                  frame = "hurt";
                  yspeed -= 3;
                  xspeed *= -0.3;
                  xspeed += mxspeed * 0.3;
               }
               else
               {
                  mxspeed = _root.Course.Char.xspeed;
                  _root.MarioHit(0,0,5,xspeed - 3 + xspeed / 2,13,"none");
                  frame = "hurt";
                  yspeed -= 3;
                  xspeed *= -0.3;
                  xspeed += mxspeed * 0.3;
               }
            }
            if(frame == "hurt" && this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attack == false || !(_root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3)) && !(Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T"))
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(0,0,0,1,0,"none");
                  xspeed -= (_root.Course.Char.xspeed + xspeed) * 0.3;
               }
               else
               {
                  _root.MarioHit(0,0,0,-1,0,"none");
                  xspeed -= (_root.Course.Char.xspeed + xspeed) * 0.3;
               }
            }
            if(frame == "hurt" && air == false && Math.abs(yspeed) < 3)
            {
               frame = "idle";
            }
            if(frame !== "hurt" && seeing == false)
            {
               if(Math.abs(xspeed) > 1)
               {
                  frame = "run";
               }
               else
               {
                  frame = "idle";
               }
            }
            if(seeing == true && frame !== "hurt")
            {
               frame = "run";
            }
            if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               HP--;
               if(HP <= 0)
               {
                  frame = "dead";
               }
               else
               {
                  yspeed = jumpy;
                  if(bounceto == undefined)
                  {
                     xspeed = (- (_X - saveX)) / 10;
                  }
                  else
                  {
                     xspeed = (- (_X - bounceto)) / 10;
                  }
                  xspeed = Math.max(xspeed,-7);
                  xspeed = Math.min(xspeed,7);
                  while(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                  {
                     _Y--;
                  }
                  frame = "hurt";
               }
            }
            clip.gotoAndStop(frame);
            xspeed = Math.max(xspeed,-15);
            xspeed = Math.min(xspeed,15);
            _X = _X + xspeed;
            _Y = _Y + yspeed;
         }
         else if(restart == false && frame !== "dead")
         {
            _Y = saveY;
            _X = saveX;
            restartvars();
            prevFrame();
            _root.PlayBGSong(_root.SongRepeat);
         }
      }
      if(frame == "dead")
      {
         deadcount++;
      }
      if((deadcount >= 30 || Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600) && restart !== false)
      {
         if(restart !== false)
         {
            _Y = saveY;
            _X = saveX;
            restartvars();
         }
      }
   }
};
