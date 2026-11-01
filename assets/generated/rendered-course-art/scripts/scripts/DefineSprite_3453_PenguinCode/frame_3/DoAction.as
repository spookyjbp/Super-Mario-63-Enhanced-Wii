stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false && Math.abs(_root.currentCamX - (_X + _parent._x)) < 1000 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 1000)
   {
      if(State == "Walk" && tumble == false && WalkDistance > 0)
      {
         if(idle == false)
         {
            if(movedirection == "right")
            {
               Moveright();
               _xscale = (- scale) * 100;
            }
            else
            {
               Moveleft();
               _xscale = scale * 100;
            }
            frame = 2;
            randomnum += Math.random() * randomWalkDistance + 1;
            if(randomnum > WalkDistance)
            {
               idle = true;
               randomnum = 0;
            }
         }
         else
         {
            frame = 1;
            xspeed = _root.groundFriction(xspeed,0.5,1.2);
            randomnum += Math.random() * randomIdleTime + 1;
            turncount += random(5) + 2;
            if(turncount > randomturncount)
            {
               randomturncount = random(80) + 20;
               turncount = 0;
               if(movedirection == "right")
               {
                  movedirection = "left";
                  _xscale = scale * 100;
               }
               else
               {
                  movedirection = "right";
                  _xscale = (- scale) * 100;
               }
            }
            if(randomnum > IdleTime)
            {
               idle = false;
               randomnum = 0;
               IdleTime = random(120) + 80;
               randomturncount = random(50) + 50;
               WalkDistance = random(120) + 80;
            }
         }
      }
      if(State == "Flying")
      {
         flyingcoswave += 0.1;
         yspeed = Math.cos(flyingcoswave) * 0.75;
         _Y = _Y + (startY - _Y) / 50;
         if(idle == false)
         {
            if(movedirection == "right")
            {
               xspeed += walkspeed / 6;
               xspeed *= 0.7;
               _xscale = (- scale) * 100;
            }
            else
            {
               xspeed -= walkspeed / 6;
               xspeed *= 0.7;
               _xscale = scale * 100;
            }
            if(yspeed > 0)
            {
               frame = "FlyingD";
            }
            else
            {
               frame = "FlyingU";
            }
            randomnum += 1;
            if(randomnum > WalkDistance && WalkDistance > 0)
            {
               idle = true;
               randomnum = 0;
            }
         }
         else
         {
            if(Math.abs(xspeed) < 1)
            {
               frame = "Turn";
            }
            else
            {
               if(yspeed > 0)
               {
                  frame = "FlyingD";
               }
               else
               {
                  frame = "FlyingU";
               }
               xspeed *= 0.8;
            }
            randomnum++;
            if(randomnum > 8)
            {
               idle = false;
               if(movedirection == "right")
               {
                  movedirection = "left";
               }
               else
               {
                  movedirection = "right";
               }
            }
         }
      }
      if(State !== "Flying")
      {
         if(!HitTest("Platforms",_X,_Y) && !HitTest("BPlatforms",_X,_Y))
         {
            yspeed += _root.gravity;
            air = true;
         }
         else
         {
            if(tumble == true)
            {
               yspeed = Math.min((- yspeed) * 0.6 + 3,0);
               xspeed *= 0.6;
            }
            else
            {
               yspeed = Math.min(yspeed,0);
            }
            air = false;
         }
      }
      while(HitTest("Platforms",_X,_Y - 1) || HitTest("BPlatforms",_X,_Y - 1))
      {
         _Y--;
      }
      while(HitTest("Platforms",_X + wide,_Y - tall / 2))
      {
         _X--;
         if(State == "Walk")
         {
            if(movedirection == "right")
            {
               movedirection = "left";
            }
         }
         xspeed = Math.min((- xspeed) * 0.7,xspeed);
      }
      while(HitTest("Platforms",_X - wide,_Y - tall / 2))
      {
         _X++;
         if(State == "Walk")
         {
            if(movedirection == "left")
            {
               movedirection = "right";
            }
         }
         xspeed = Math.max((- xspeed) * 0.7,xspeed);
      }
      while(HitTest("Platforms",_X,_Y - tall))
      {
         _Y++;
      }
      if(State == "Walk")
      {
         if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water))
         {
            if(_root.Course.Char.attackFrame == "Spin" && _root.Course.Char.attack == true)
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.Course.Char.xspeed += 2;
                  xspeed -= 3;
                  xspeed *= 0.8;
               }
               else
               {
                  _root.Course.Char.xspeed -= 2;
                  xspeed += 3;
                  xspeed *= 0.8;
               }
            }
         }
      }
      if(this.body.hitTest(_root.Course.Char.charClip.body) && (State !== "Flying" && Math.abs(_root.Course.Char.xspeed) >= 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T" || _root.Invincible == true && _root.PowerTimer > 0))
      {
         tumble = true;
         rotspeed += _root.Course.Char.xspeed;
         rotspeed *= 0.8;
         xspeed += (_root.Course.Char.xspeed - xspeed) / 2;
         xspeed *= 0.4;
         yspeed -= 5;
         yspeed *= 0.7;
      }
      if(tumble == true && State == "Flying")
      {
         State = "Walk";
      }
      if((this.body.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.air == false || this.body.hitTest(_root.Course.Char.charClip.head) && _root.Course.Char.air == true) && tumble == false)
      {
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("MarioImpact");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
         if(_root.Course.Char._x > _X + _parent._x)
         {
            if(air == false && State == "Walk")
            {
               xspeed += _root.Course.Char.xspeed / 2;
            }
            _root.Course.Char.xspeed *= -0.5;
            _root.Course.Char.xspeed += 7;
            _root.Course.Char.yspeed -= 5;
            _root.Course.Char.yspeed *= 0.5;
         }
         else
         {
            if(air == false && State == "Walk")
            {
               xspeed += _root.Course.Char.xspeed / 2;
            }
            _root.Course.Char.xspeed *= -0.5;
            _root.Course.Char.xspeed -= 7;
            _root.Course.Char.yspeed -= 5;
            _root.Course.Char.yspeed *= 0.5;
         }
      }
      if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 1 && !(_root.Invincible == true && _root.PowerTimer > 0))
      {
         _root.Course.Char.yspeed *= -0.5;
         _root.Course.Char.yspeed -= 5;
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("MarioImpact");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
      }
      if(air == false && Math.abs(xspeed) < 3)
      {
         tumble = false;
      }
      xspeed = Math.max(xspeed,-12);
      xspeed = Math.min(xspeed,12);
      _Y = _Y + yspeed;
      _X = _X + xspeed;
      if(tumble == true)
      {
         frame = "Tumble";
         rot = _rotation;
         rot += rotspeed;
      }
      else
      {
         if(frame == "Tumble")
         {
            frame = 1;
         }
         rot *= 0.8;
      }
      clip.frame._rotation = rot;
      clip.gotoAndStop(frame);
   }
};
