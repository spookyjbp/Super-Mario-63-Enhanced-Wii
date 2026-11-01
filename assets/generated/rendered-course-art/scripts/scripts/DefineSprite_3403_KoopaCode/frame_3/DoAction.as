stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false && Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
   {
      _yscale = scale * 100;
      if(movedirection == "left")
      {
         _xscale = scale * 100;
      }
      else
      {
         _xscale = (- scale) * 100;
      }
      if(State == "Flying")
      {
      }
      if(State == "Walk")
      {
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(movedirection == "right")
            {
               Moveright();
            }
            else
            {
               Moveleft();
            }
            if(frame == "Turn")
            {
               if(movedirection == "right")
               {
                  xspeed = -0.8571428571428571;
               }
               else
               {
                  xspeed = 0.8571428571428571;
               }
            }
            else
            {
               randomnum++;
            }
            if(randomnum > WalkDistance)
            {
               frame = "Turn";
               randomnum = 0;
            }
            xspeed *= 0.95;
         }
      }
      if(State == "Shell")
      {
         xspeed *= 0.99;
      }
      mariowater = this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water) || this.head.hitTest(_root.Course.Char.charClip.water);
      marioinvincible = _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3 || Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T";
      if(State == "Flying" && this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
      {
         xspeed += _root.Course.Char.xspeed / 1.5;
         yspeed += _root.Course.Char.yspeed / 1.5;
         State = "Shell";
      }
      if(State == "Walk" && this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
      {
         xspeed += _root.Course.Char.xspeed / 1.5;
         yspeed -= 5;
         _Y = _Y - 5;
         State = "Shell";
      }
      if(State == "Shell" && this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            xspeed += _root.Course.Char.yspeed / 2;
         }
         else
         {
            xspeed -= _root.Course.Char.yspeed / 2;
         }
         xspeed += _root.Course.Char.xspeed / 1.5;
      }
      if((State == "Flying" || State == "Walk" || State == "Shell") && mariowater)
      {
         State = "Shell";
         yspeed -= 1.5 / scale;
         if(_root.Course.Char.attackFrame == "Spin" && _root.Course.Char.attack == true)
         {
            if(_root.Course.Char._x > _X + _parent._x)
            {
               xspeed *= 0.7;
               xspeed -= 20 / scale;
            }
            else
            {
               xspeed *= 0.7;
               xspeed += 20 / scale;
            }
         }
         else if(_root.Course.Char._xscale > 0)
         {
            if(_root.Fluddpow == "T" || _root.Fluddpow == "H" && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true)
            {
               xspeed *= 0.9;
               xspeed += 6 / scale;
            }
            else
            {
               xspeed *= 0.9;
               xspeed -= 3 / scale;
            }
         }
         else if(_root.Fluddpow == "T" || _root.Fluddpow == "H" && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true)
         {
            xspeed *= 0.9;
            xspeed -= 6 / scale;
         }
         else
         {
            xspeed *= 0.9;
            xspeed += 3 / scale;
         }
      }
      if(State == "Flying" && this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
      {
         _root.Course.Char.yspeed /= 2;
         _root.Course.Char.yspeed -= 15;
         State = "Walk";
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("flippers");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
      }
      if(State == "Walk" && this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
      {
         _root.Course.Char.yspeed /= 2;
         _root.Course.Char.yspeed -= 12;
         xspeed = 0;
         State = "Shell";
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("flippers");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
      }
      if(State == "Shell" && this.body.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
      {
         if(air == true)
         {
            yspeed += (_root.Course.Char.yspeed - yspeed) * 0.5;
         }
         _root.Course.Char.yspeed /= 4;
         _root.Course.Char.yspeed -= 12;
         if(_root.Course.Char._x > _X + _parent._x)
         {
            xspeed += _root.Course.Char.yspeed / 1.5;
         }
         else
         {
            xspeed -= _root.Course.Char.yspeed / 1.5;
         }
         xspeed += _root.Course.Char.xspeed / 2;
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("flippers");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
      }
      if(State == "Flying" && this.body.hitTest(_root.Course.Char.charClip.body) && !marioinvincible)
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(1,90,-5,7,7,"");
         }
         else
         {
            _root.MarioHit(1,90,-5,-7,7,"");
         }
      }
      if(State == "Walk" && this.body.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.frame !== "" && !marioinvincible)
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(1,90,-5,7,7,"");
         }
         else
         {
            _root.MarioHit(1,90,-5,-7,7,"");
         }
      }
      if(State == "Shell" && (this.body.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.air == false || this.body.hitTest(_root.Course.Char.charClip.head) && _root.Course.Char.air == true) && !marioinvincible)
      {
         xspeed += (_root.Course.Char.xspeed - xspeed) / 2;
         if(_root.Course.Char._x > _X + _parent._x)
         {
            xspeed -= 3;
            if(Math.abs(xspeed) > 2)
            {
            }
         }
         else
         {
            xspeed += 3;
            if(Math.abs(xspeed) > 2)
            {
            }
         }
      }
      if(State == "Flying" || State == "Walk")
      {
         if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            yspeed -= 1.4 / scale;
            xspeed += (Math.random() * 4 - 2 - xspeed / 1.5) / scale;
            _Y = _Y - 3;
            State = "Shell";
         }
         else
         {
            if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += Math.max((5 - xspeed / 1.5) / scale,0);
               _Y = _Y - 3;
               State = "Shell";
            }
            if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += Math.min((-5 - xspeed / 1.5) / scale,0);
               _Y = _Y - 3;
               State = "Shell";
            }
         }
      }
      if(State == "Walk" && _root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         if(underwater == false)
         {
            State = "Shell";
         }
         else
         {
            xspeed *= 0.9;
         }
      }
      if(State == "Shell")
      {
         count = 0;
         while(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - count + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            count++;
            if(count > 7)
            {
               break;
            }
         }
         if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - count + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(!_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 8) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed *= 0.9;
               yspeed -= count / 7;
            }
            if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y = _Y - 5;
            }
            xspeed *= 0.85;
         }
      }
      if(State == "Walk" || State == "Shell")
      {
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            yspeed = 0;
         }
         else
         {
            yspeed += _root.gravity;
            if(yspeed > 7)
            {
               yspeed *= 0.9;
            }
         }
      }
      while(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X--;
         if(State == "Walk")
         {
            if(movedirection == "right")
            {
               frame = "Turn";
            }
         }
         if(State == "Shell")
         {
            xspeed = Math.min((- xspeed) * 0.7,xspeed);
         }
      }
      while(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X++;
         if(State == "Walk")
         {
            if(movedirection == "left")
            {
               frame = "Turn";
            }
         }
         if(State == "Shell")
         {
            xspeed = Math.max((- xspeed) * 0.7,xspeed);
         }
      }
      if(State !== "Flying")
      {
         while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
         {
            _Y++;
         }
      }
      while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y--;
      }
      if(State !== "Shell")
      {
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 30) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y++;
         }
      }
      else
      {
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 20) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y++;
         }
      }
      if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         frame = "Shell";
         State = "Shell";
      }
      if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         yspeed -= 0.1;
         yspeed *= 0.8;
         yspeed -= _root.gravity;
         if(air == false)
         {
            _Y - 3;
         }
      }
      if(State == "Flying")
      {
         frame = "Flying";
      }
      if(State == "Walk" && frame !== "Turn")
      {
         frame = "Walk";
      }
      if(State == "Shell")
      {
         frame = "Shell";
         if(Math.abs(xspeed) > 1)
         {
            _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y - 7,_yscale:15,_xscale:15,time:1});
         }
      }
      xspeed = Math.max(xspeed,-12);
      xspeed = Math.min(xspeed,12);
      _Y = _Y + yspeed;
      _X = _X + xspeed;
      clip.gotoAndStop(frame);
   }
   if((Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600))
   {
      _Y = saveY;
      _X = saveX;
      restartvars();
   }
};
