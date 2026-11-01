stop();
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("BowserGround","BowserGround" + depth,depth,{_x:_X,_y:_Y});
X = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(!(HP <= 0 && attack == true && attackFrame == "recover"))
      {
         _root.Camcenter = _root.Course.BackGFX.camCenter;
         if(attack == false || attackFrame !== "spin")
         {
            if(_root.CameraZoom < 100 - Math.max(_root.Course.BackGFX.distance / 3 - 100,0))
            {
               _root.CameraZoom = _root.CameraZoom + 1;
            }
            if(_root.CameraZoom > 95 - Math.max(_root.Course.BackGFX.distance / 3 - 100,0))
            {
               _root.CameraZoom = _root.CameraZoom - 1;
            }
         }
         if(attack == true && attackFrame == "spin" && (_root.Course.Char.attack !== true || !(_root.Course.Char.attackFrame == "DiveSpin" || _root.Course.Char.attackFrame == "JumpSpin")))
         {
            attack = false;
         }
         if((attack == false || attackFrame !== "spin") && attackFrame !== "tossed" && attackFrame !== "hurt" && _root.Course.Char.attack == true && (_root.Course.Char.attackFrame == "DiveSpin" || _root.Course.Char.attackFrame == "JumpSpin"))
         {
         }
         if(attack != true)
         {
            idlecount++;
            if(difficulty == 2)
            {
               idlecount += 0.5;
            }
            idlecounts = idlecount / _root.framerate;
         }
         while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
         {
            _Y++;
         }
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
         }
         while(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X--;
         }
         while(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X++;
         }
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(air == true)
            {
               idlecount = 0;
               idlecounts = 0;
            }
            if(air == true && shockwave == true)
            {
               attack = true;
               attackFrame = "Shockwave";
               shockwave = false;
            }
            air = false;
            gettingbackoncourse = true;
         }
         else
         {
            if(air == false)
            {
               idlecount = 0;
               idlecounts = 0;
            }
            air = true;
         }
         if(air == false)
         {
            if(attack == false || (attackFrame !== "tossed" && attackFrame !== "hurt" && attackFrame !== "defeated" || attackFrame !== "spin"))
            {
               frame = 1;
               yspeed = Math.min(yspeed,0);
               xspeed = _root.groundFriction(xspeed,1,1.1);
            }
            else if(yspeed > 3)
            {
               yspeed = Math.min((- yspeed) * 0.3,0);
               xspeed = _root.groundFriction(xspeed,1,1.1);
            }
            else if(yspeed > -3)
            {
               yspeed = 0;
               xspeed = _root.groundFriction(xspeed,1,1.1);
            }
            else
            {
               yspeed += 0.5;
            }
         }
         if(air == true)
         {
            _X = _X - X;
            if(Math.abs(yspeed) < -7 || Math.abs(yspeed) > 7)
            {
               yspeed += 0.1;
            }
            else
            {
               yspeed += 0.3;
            }
            if(yspeed > 3)
            {
               frame = 4;
            }
            else
            {
               frame = 3;
            }
            if(Math.abs(yspeed) > 7)
            {
               if(difficulty !== 3 || gettingbackoncourse == true)
               {
                  yspeed = _root.groundFriction(yspeed,0.3,1.1);
               }
            }
         }
         if(attack == false || (attackFrame !== "tossed" && attackFrame !== "hurt" && attackFrame !== "defeated" || attackFrame !== "spin"))
         {
            if(this.lbod.hitTest(_root.Course.Char.charClip.body))
            {
               if(_xscale > 0)
               {
                  _root.MarioHit(1,30,-5,-7,7,"");
               }
               else
               {
                  _root.MarioHit(1,30,-5,7,7,"");
               }
            }
            if(this.rbod.hitTest(_root.Course.Char.charClip.body))
            {
               if(_xscale > 0)
               {
                  _root.MarioHit(2,30,-5,7,7,"");
               }
               else
               {
                  _root.MarioHit(2,30,-5,-7,7,"");
               }
            }
            if(this.spikes.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(2,30,-9,3,7,"");
               }
               else
               {
                  _root.MarioHit(2,30,-9,-3,7,"");
               }
            }
            if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
            {
               if(_xscale > 0)
               {
                  _root.MarioHit(2,30,-5,-2,7,"");
               }
               else
               {
                  _root.MarioHit(2,30,-5,2,7,"");
               }
            }
            if(!this.feet.hitTest(_root.Course.Char.charClip.body))
            {
            }
            if(blink < 0)
            {
               blink++;
               if(blink % 2 == 0)
               {
                  _alpha = 20;
               }
               else
               {
                  _alpha = 80;
               }
            }
            else
            {
               _alpha = 100;
            }
            if(this.tail.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.air == false && blink >= 0)
            {
               if(attackFrame !== "turn" || attack == false)
               {
                  _root.Course.Char.attack = true;
                  if(difficulty !== 3)
                  {
                     _root.Course.Char.attackFrame = "DiveSpin";
                  }
                  else
                  {
                     _root.Course.Char.attackFrame = "JumpSpin";
                  }
                  attack = true;
                  attackFrame = "spin";
                  _root.Course.Char._x = _root.Course.FrontGFX._x + _X + _xscale / 100 * (wide / 2);
                  _root.Course.Char._y = _root.Course.FrontGFX._y + _Y;
                  _root.bgsoundFX = new Sound(this);
                  _root.bgsoundFX.attachSound("Bowser - Grab");
                  _root.bgsoundFX.start(0,1);
                  _root.bgsoundFX.setVolume(_root.BgVolume);
               }
            }
         }
         if(idlecounts > 3 && attack == false && air == false)
         {
            if(_xscale > 0 && _X + _parent._x - _root.Course.Char._x < -10)
            {
               attack = true;
               attackFrame = "turn";
            }
            else if(_xscale < 0 && _X + _parent._x - _root.Course.Char._x > 10)
            {
               attack = true;
               attackFrame = "turn";
            }
         }
         if(attack == false && air == false)
         {
            randomnum = random(48) + 1;
            if(randomnum == 1)
            {
               if(_xscale < 0 && _X + _parent._x - _root.Course.Char._x < -10)
               {
                  attack = true;
                  attackFrame = "step";
               }
               else if(_xscale > 0 && _X + _parent._x - _root.Course.Char._x > 10)
               {
                  attack = true;
                  attackFrame = "step";
               }
            }
         }
         if(idlecounts > 3 && attack == false && air == false)
         {
            randomnum = random(9) + 1;
            if(difficulty == 2)
            {
               if(_root.Course.Char.yspeed < 0)
               {
                  X = _X + _root.Course.FrontGFX._x;
                  X -= _root.Course.Char._x;
                  if(random(4) == 1 && Math.abs(X) < 200)
                  {
                     randomnum = 8;
                  }
               }
               if(randomnum >= 8)
               {
                  if(random(2) == 1)
                  {
                     randomnum = random(9) + 1;
                  }
               }
            }
            if(_root.Course.Char._y < _Y - _root.Course.FrontGFX._y)
            {
               randomnumber = random(8) + 1;
               if(randomnumber >= 3)
               {
                  randomnum = 8;
               }
            }
            if(randomnum == 1 || randomnum == 2 || randomnum == 3)
            {
               attack = true;
               attackFrame = "fire1";
            }
            else if(randomnum == 4 || randomnum == 5)
            {
               attack = true;
               attackFrame = "fire2";
            }
            else
            {
               attack = true;
               attackFrame = "Jumping";
            }
         }
         if(idlecounts > 0.3 && attack == false && air == true && yspeed < 0)
         {
            randomnum = random(4) + 1;
            if(randomnum <= 2)
            {
               attack = true;
               attackFrame = "fire1air";
            }
            else if(randomnum == 3)
            {
               attack = true;
               attackFrame = "fire2air";
            }
            else
            {
               attack = true;
               attackFrame = "fire3air";
            }
         }
         if(attack == true && attackFrame == "tossed")
         {
            if(air == false && (Math.abs(yspeed) < 3 || Math.abs(xspeed) < 3))
            {
               attack = false;
            }
            if(_root.Course.BackGFX.Bomb.hitTest(0,0,false) && gettingbackoncourse == false)
            {
               attack = true;
               attackFrame = "hurt";
               xspeed = (- _xscale / 100) * 15;
            }
         }
         if(difficulty !== 3)
         {
            if(this.body.hitTest(_root.Course.BackGFX.bomb1) && _root.Course.BackGFX.bombgfx1._currentframe == 1 && attackFrame == "tossed" && attack == true)
            {
               attack = true;
               xspeed *= -0.5;
               xspeed += 5;
               xspeed = Math.min(xspeed,8);
               xspeed = Math.max(xspeed,-8);
               yspeed = -7;
               attackFrame = "hurt";
               HP -= 1;
               _root.Course.BackGFX.bombgfx1.gotoAndStop(2);
            }
            if(this.body.hitTest(_root.Course.BackGFX.bomb2) && _root.Course.BackGFX.bombgfx2._currentframe == 1 && attackFrame == "tossed" && attack == true)
            {
               attack = true;
               xspeed *= -0.5;
               xspeed -= 5;
               xspeed = Math.min(xspeed,8);
               xspeed = Math.max(xspeed,-8);
               yspeed = -7;
               attackFrame = "hurt";
               HP -= 1;
               _root.Course.BackGFX.bombgfx2.gotoAndStop(2);
            }
         }
         else
         {
            i = 1;
            while(i < 6)
            {
               if(this.body.hitTest(_root.Course.BackGFX["bomb" + i]) && _root.Course.BackGFX["bombgfx" + i]._currentframe == 1 && attackFrame == "tossed" && attack == true && gettingbackoncourse == false)
               {
                  attack = true;
                  xspeed *= -0.5;
                  if(_X > 0)
                  {
                     xspeed -= 5;
                  }
                  else
                  {
                     xspeed += 5;
                  }
                  xspeed = Math.min(xspeed,8);
                  xspeed = Math.max(xspeed,-8);
                  yspeed = -4;
                  attackFrame = "hurt";
                  HP -= 1;
                  _root.Course.BackGFX["bombgfx" + i].gotoAndStop(2);
               }
               i++;
            }
         }
         if(this.feet.hitTest(_root.Course.BackGFX.falloff) || Math.abs(_X) > 1000 || _Y > 1000)
         {
            gettingbackoncourse = true;
            idlecount = 0;
            idlecounts = 0;
            shockwave = true;
            yspeed = -35;
            _Y = 200;
            if(_X > 0)
            {
               _xscale = 100;
               if(difficulty !== 2)
               {
                  _X = 500;
                  xspeed = -5;
               }
               else
               {
                  _X = 300;
                  xspeed = -3;
               }
            }
            else
            {
               _xscale = -100;
               if(difficulty !== 2)
               {
                  _X = -500;
                  xspeed = 5;
               }
               else
               {
                  _X = -300;
                  xspeed = 3;
               }
            }
            _root.bgsoundFX = new Sound(this);
            _root.bgsoundFX.attachSound("Bowser - Grab");
            _root.bgsoundFX.start(0,1);
            _root.bgsoundFX.setVolume(_root.BgVolume);
         }
         if(attackFrame == "hurt" && attack == true && air == false)
         {
            xspeed *= 0.9;
            attack = true;
            attackFrame = "recover";
         }
         _Y = _Y + yspeed;
         _X = _X + xspeed;
         if(attack == true && attackFrame == "spin")
         {
            _root.Course.Platforms["BowserGround" + depth]._y = _Y + 1000;
         }
         else
         {
            _root.Course.Platforms["BowserGround" + depth]._y = _Y;
         }
         _root.Course.Platforms["BowserGround" + depth]._x = _X;
      }
      else if(!isNaN(_root.Course.Platforms["BowserGround" + depth]._y))
      {
         _root.Course.Platforms["BowserGround" + depth].removeMovieClip();
      }
      if(attack == false)
      {
         clip.gotoAndStop(frame);
      }
      else
      {
         clip.gotoAndStop(attackFrame);
      }
   }
};
