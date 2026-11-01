stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false && Math.abs(_root.currentCamX - (_X + _parent._x)) < 1000 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 1000)
   {
      if(State !== "Squish")
      {
         _yscale = scale * 100;
         if(movedirection == "left")
         {
            _xscale = (- scale) * 100;
         }
         else
         {
            _xscale = scale * 100;
         }
         if(State == "Flying")
         {
         }
         if(State == "Walk")
         {
            if(idle == false)
            {
               if(movedirection == "right")
               {
                  if(State == "Walk")
                  {
                     Moveright();
                  }
               }
               else if(State == "Walk")
               {
                  Moveleft();
               }
               randomnum += Math.random() * randomWalkDistance + 1;
               if(randomnum > WalkDistance)
               {
                  idle = true;
                  randomnum = 0;
               }
            }
            else
            {
               xspeed = _root.groundFriction(xspeed,0.5,1.2);
               randomnum += Math.random() * randomIdleTime + 1;
               if(randomnum > IdleTime)
               {
                  idle = false;
                  randomnum = 0;
                  if(movedirection == "right")
                  {
                     movedirection = "left";
                     _xscale = (- scale) * 100;
                  }
                  else
                  {
                     movedirection = "right";
                     _xscale = scale * 100;
                  }
               }
            }
         }
         if(State == "Flying" && airspeed > 0)
         {
            if(movedirection == "right")
            {
               xspeed += (airspeed - xspeed) / 5;
            }
            else
            {
               xspeed += (- airspeed - xspeed) / 5;
            }
            randomnum++;
            if(randomnum > WalkDistance)
            {
               if(movedirection == "right")
               {
                  movedirection = "left";
                  _xscale = (- scale) * 100;
               }
               else
               {
                  movedirection = "right";
                  _xscale = scale * 100;
               }
               randomnum = 0;
            }
         }
         if(State == "Walk")
         {
            xspeed *= 0.95;
         }
         mariowater = this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water) || this.head.hitTest(_root.Course.Char.charClip.water);
         marioinvincible = _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3 || Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T";
         if(State == "Flying" && this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
         {
            xspeed += _root.Course.Char.xspeed / 2.5;
            yspeed += _root.Course.Char.yspeed / 5;
            rotspeed = _root.Course.Char.xspeed * 2;
            State = "Tumble";
            if(Math.abs(xspeed / (scale / 2)) > 17)
            {
               powerhit = true;
            }
         }
         if(State == "Walk" && yspeed > -3 && this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
         {
            xspeed += _root.Course.Char.xspeed / 1.5;
            yspeed -= 5;
            _Y = _Y - 5;
            rotspeed = _root.Course.Char.xspeed * 2;
            State = "Tumble";
            if(Math.abs(xspeed / (scale / 2)) > 23)
            {
               powerhit = true;
            }
         }
         if(State == "Tumble" && yspeed > -3 && this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
         {
            xspeed += _root.Course.Char.xspeed / 1.5;
            yspeed -= 5;
            _Y = _Y - 5;
            rotspeed = _root.Course.Char.xspeed * 2;
            State = "Tumble";
            if(Math.abs(xspeed / (scale / 2)) > 14)
            {
               powerhit = true;
            }
         }
         if((State == "Flying" || State == "Walk" || State == "Tumble") && mariowater)
         {
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
            State = "Tumble";
            if(Math.abs(xspeed / (scale / 2)) > 21)
            {
               powerhit = true;
            }
         }
         if(State == "Flying" && this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
         {
            _root.Course.Char.yspeed /= 2;
            _root.Course.Char.xspeed /= 2;
            _root.Course.Char.yspeed -= 15;
            _root.Course.Char.attack = true;
            _root.Course.Char.attackFrame = "Spin";
            _root.Course.Char.charClip.speed = 20;
            State = "Walk";
         }
         if(State == "Walk" && this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
         {
            xspeed = 0;
            bounce = true;
            State = "Squish";
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
         if(State == "Flying" || State == "Walk")
         {
            if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += (Math.random() * 4 - 2 - xspeed / 1.5) / scale;
               _Y = _Y - 3;
               rotspeed = xspeed * 2;
               State = "Tumble";
               powerhit = true;
            }
            else
            {
               if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  yspeed -= 1.4 / scale;
                  xspeed += Math.max((9 - xspeed / 1.5) / scale,0);
                  _Y = _Y - 3;
                  rotspeed = xspeed * 2;
                  State = "Tumble";
                  powerhit = true;
               }
               if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  yspeed -= 1.4 / scale;
                  xspeed += Math.min((-9 - xspeed / 1.5) / scale,0);
                  _Y = _Y - 3;
                  rotspeed = xspeed * 2;
                  State = "Tumble";
                  powerhit = true;
               }
            }
         }
         if(State !== "Flying" && _root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            State = "Squish";
            xspeed = 0;
            yspeed = 0;
         }
         if(State !== "Flying" && _root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            State = "Squish";
            xspeed = 0;
            yspeed = 0;
         }
         if(State == "Walk" || State == "Tumble")
         {
            if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               if(State == "Tumble")
               {
                  xspeed *= 0.9;
                  if(yspeed > 3 && powerhit == true)
                  {
                     State = "Squish";
                     rot = 0;
                  }
                  else if(Math.abs(xspeed) < 3)
                  {
                     State = "Walk";
                  }
               }
               yspeed = 0;
               air = false;
            }
            else
            {
               yspeed += _root.gravity;
               if(yspeed > 7)
               {
                  yspeed *= 0.9;
               }
               air = true;
            }
         }
         while(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X--;
            if(State == "Walk")
            {
               if(movedirection == "right")
               {
                  movedirection = "left";
               }
            }
            if(State == "Tumble" && Math.abs(xspeed) > 3 && powerhit == true)
            {
               State = "Squish";
               rot = -90;
            }
         }
         while(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X++;
            if(State == "Walk")
            {
               if(movedirection == "left")
               {
                  movedirection = "right";
               }
            }
            if(State == "Tumble" && Math.abs(xspeed) > 3 && powerhit == true)
            {
               State = "Squish";
               rot = 90;
            }
         }
         if(State !== "Flying")
         {
            while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
            {
               _Y++;
               air = false;
            }
         }
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
            air = false;
         }
         if(State == "Flying")
         {
            frame = "Flying";
         }
         if(State == "Walk" && frame !== "Turn")
         {
            if(air == false)
            {
               if(idle == false)
               {
                  frame = "Walk";
               }
               else
               {
                  frame = "Idle";
               }
            }
            else
            {
               frame = "Jump";
            }
         }
         if(State == "Tumble")
         {
            frame = "Tumble";
            rot += rotspeed;
            rotspeed *= 0.95;
         }
         else if(State !== "Squish")
         {
            rotspeed = 0;
            rot = _root.groundFriction(rot,2,1.1);
         }
         clip.frame._rotation = rot;
         yspeed = Math.max(yspeed,-12);
         yspeed = Math.min(yspeed,12);
         xspeed = Math.max(xspeed,-7);
         xspeed = Math.min(xspeed,7);
         _Y = _Y + yspeed;
         _X = _X + xspeed;
      }
      else
      {
         clip.frame._rotation = rot;
         frame = "Squish";
      }
      clip.gotoAndStop(frame);
   }
   if(State == "Squish")
   {
      deadcount++;
   }
   if((deadcount > 30 || Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600))
   {
      _Y = saveY;
      _X = saveX;
      restartvars();
   }
};
