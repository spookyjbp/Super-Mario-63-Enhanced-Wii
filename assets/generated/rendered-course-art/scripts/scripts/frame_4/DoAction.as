_root.UnpausedCharEngine = function()
{
   tellTarget(_root.Course.Char)
   {
      if(_root.playpause == true)
      {
         _root.Controll = false;
      }
      blink = Math.round(blink);
      blink = Math.max(blink,0);
      StunTime = Math.round(StunTime);
      StunTime = Math.max(StunTime,0);
      if(StunTime > 0)
      {
         StunTime--;
         if(Water == true)
         {
            StunTime -= 3;
         }
         else
         {
            _root.Controll = false;
         }
      }
      if(blink > 0)
      {
         blink--;
         if(blink / 3 == Math.round(blink / 3))
         {
            charClip.frame._alpha = 20;
         }
         else
         {
            charClip.frame._alpha = 70;
         }
      }
      else
      {
         charClip.frame._alpha = 100;
      }
      if(yspeed > -3)
      {
         lockon = true;
      }
      else
      {
         lockon = false;
      }
      if(_root.CanSwitchFludd == true)
      {
         if(!_root.KeySHIFT())
         {
            canChangeFludd = true;
         }
         if(_root.KeySHIFT() && canChangeFludd == true)
         {
            changeFludd = true;
            canChangeFludd = false;
         }
         else
         {
            changeFludd = false;
         }
         if(attack == false)
         {
            _root.ChangeFludd();
         }
      }
      if(_root.newstar == true && !(attackFrame == "Star" && attack == true) && _root.Playinglevel == 0)
      {
         attack = true;
         attackFrame = "ExitPainting";
      }
      if(attackFrame == "Star")
      {
         _root.CameraZoom = 100;
         if(yspeed > 7)
         {
            yspeed *= 0.95;
         }
      }
      if(attackFrame == "frontflip" && attack == true && !_root.Fluddpow == "")
      {
         attack = false;
      }
      if(_root.LastFrameJumpPress == false && _root.KeyUP())
      {
         _root.JumpPress = 7;
      }
      else
      {
         _root.JumpPress = _root.JumpPress - 1;
      }
      _root.LastFrameJumpPress = _root.KeyUP();
      if(air == true)
      {
         if(Water == true)
         {
            if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _root.Course._y,true))
            {
               if(attackFrame == "Star")
               {
                  yspeed += _root.gravity / 3;
               }
               else if(_root.Metal !== true)
               {
                  yspeed += _root.gravity / 3;
               }
               else
               {
                  yspeed += _root.gravity / 2;
               }
            }
            yspeed = _root.groundFriction(yspeed,0.05,1.01);
         }
         else
         {
            yspeed += _root.gravity;
            if(_root.PowerTimer && _root.WingCap == true && yspeed > 3)
            {
               yspeed -= _root.gravity / 6;
               fallingyspeed = _root.groundFriction(yspeed,_root.gravity / 4.5,1.07);
            }
            else
            {
               fallingyspeed = _root.groundFriction(yspeed,_root.gravity / 5,1.05);
            }
            if(yspeed > 0)
            {
               if(attack == false && yspeed > 7)
               {
                  fallcount++;
                  if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
                  {
                     fallcount = 0;
                     hurtfallcount--;
                     hurtfallcount = Math.max(hurtfallcount,0);
                  }
                  if(fallingyspeed >= 15 && air == true && yspeed > 15 && fallingsfx !== true)
                  {
                     hurtfallcount++;
                     hurtfallcount = Math.min(hurtfallcount,7);
                     if(hurtfallcount >= 5)
                     {
                        fallingsfx = true;
                        _root.Mariosound = new Sound(this);
                        _root.Mariosound.attachSound("Mario-Fall");
                        _root.Mariosound.start(0,1);
                        _root.Mariosound.setVolume(_root.MarioVolume);
                     }
                  }
               }
               else
               {
                  fallcount = Math.round(fallcount / 2);
               }
               yspeed = _root.groundFriction(yspeed,_root.gravity / 5,1.05);
            }
            else
            {
               fallcount = 0;
               hurtfallcount--;
               hurtfallcount = Math.max(hurtfallcount,0);
            }
         }
         yspeed = _root.groundFriction(yspeed,0,1.001);
         xspeed = _root.groundFriction(xspeed,0,1.001);
         if(attack == false)
         {
            if(Water == false)
            {
               if(yspeed > 2)
               {
                  frame = 4;
               }
               else
               {
                  frame = 3;
               }
            }
            if(!_root.KeyDOWN() && !_root.KeyZ() || air == false)
            {
               divewaitcount -= 3;
            }
            divewaitcount -= 1;
            if(_root.KeyZ() && (Water == false || _root.Metal == true) && divewaitcount <= 0 && !_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed = 0;
               xspeed = 0;
               attack = true;
               attackFrame = "Ground";
            }
            if(_root.KeyDOWN() && (Water == false || _root.Metal == true) && divewaitcount <= 0)
            {
               attack = true;
               if(_xscale < 0)
               {
                  xspeed -= (xspeed - _root.Divespeed) / 5;
               }
               else
               {
                  xspeed -= (xspeed + _root.Divespeed) / 5;
               }
               if(_root.WingCap == true)
               {
                  attackFrame = "Flying";
                  yspeed *= 0.5;
               }
               else
               {
                  attackFrame = "Dive";
                  yspeed += 3;
                  if(_root.CurrentPlayer == "Mario")
                  {
                     soundj = "Mario-Dive";
                  }
                  if(_root.CurrentPlayer == "Luigi")
                  {
                     if(random(2) == 1)
                     {
                        soundj = "Luigi - Yah!";
                     }
                     else
                     {
                        soundj = "Luigi - Yuh!";
                     }
                  }
                  _root.Mariosound = new Sound(this);
                  _root.Mariosound.attachSound(soundj);
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
               }
            }
            if(_root.KeyRIGHT() && _root.Controll == true)
            {
               xspeed -= Math.min((xspeed - _root.airwalkspeed) / (_root.airmaxWalkspeed / 3),0);
               _xscale = -100;
               if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
               {
                  _rotation = _rotation + 5;
                  xspeed -= Math.min((xspeed - _root.airwalkspeed) / _root.airmaxWalkspeed,0);
               }
            }
            if(_root.KeyLEFT() && _root.Controll == true)
            {
               xspeed -= Math.max((xspeed + _root.airwalkspeed) / (_root.airmaxWalkspeed / 3),0);
               _xscale = 100;
               if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
               {
                  _rotation = _rotation - 5;
                  xspeed -= Math.max((xspeed + _root.airwalkspeed) / _root.airmaxWalkspeed,0);
               }
            }
            if(_root.KeyTapRIGHT() && !_root.KeyRIGHT() && _root.Controll == true)
            {
               xspeed -= Math.min((xspeed - _root.airtapspeed) / (_root.airmaxtapWalkspeed / 3),0);
               _xscale = -100;
               if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
               {
                  _rotation = _rotation + 5;
                  xspeed -= Math.min((xspeed - _root.airtapspeed) / _root.airmaxtapWalkspeed,0);
               }
            }
            if(_root.KeyTapLEFT() && !_root.KeyLEFT() && _root.Controll == true)
            {
               xspeed -= Math.max((xspeed + _root.airtapspeed) / (_root.airmaxtapWalkspeed / 3),0);
               _xscale = 100;
               if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
               {
                  _rotation = _rotation - 5;
                  xspeed -= Math.max((xspeed + _root.airtapspeed) / _root.airmaxtapWalkspeed,0);
               }
            }
         }
         if(_root.KeyUP() && !_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - 12) * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true) && yspeed < 1 && yspeed > -5 && _root.Controll == true && Squish == false)
         {
            lockon = false;
            yspeed *= 0.7;
            yspeed -= _root.jumpspeed / 1.5;
            _root.Mariosound = new Sound(this);
            _root.Mariosound.attachSound("sound - Splash");
            _root.Mariosound.start(0,1);
            _root.Mariosound.setVolume(_root.MarioVolume);
            _root.Course.FrontGFX.attachMovie("Med-Splash","Med-Splash" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X - _root.Course.FrontGFX._x,_y:_Y - _root.Course.FrontGFX._y});
            frame = 3;
         }
         runcount = 0;
         runtap = "";
      }
      else
      {
         fallingsfx = false;
         doublejumpcount--;
         doublejumpcount = Math.max(doublejumpcount,0);
         if(doublejumpcount == 0)
         {
            doublejump = 0;
         }
         if(_root.Course.Ice.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _root.Course._y,true))
         {
            Ice = true;
         }
         else
         {
            Ice = false;
         }
         if(_root.Course.Puddle.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _root.Course._y,true))
         {
            Puddle = true;
         }
         else
         {
            Puddle = false;
         }
         if(frame == 3 || frame == 4)
         {
            frame = 1;
         }
         _root.FallSquish();
         yspeed = Math.min(0,yspeed);
         if(Puddle == false && Ice == false && !(attackFrame == "Dive" && attack == true))
         {
            if(StunTime == 0)
            {
               xspeed = _root.groundFriction(xspeed,0.3,1.15);
            }
            else
            {
               xspeed = _root.groundFriction(xspeed,0.15,1.075);
            }
         }
         else
         {
            if(attackFrame == "Dive" && attack == true && Ice == false && Puddle == false)
            {
               if(StunTime == 0)
               {
                  xspeed = _root.groundFriction(xspeed,0.2,1.05);
               }
               else
               {
                  xspeed = _root.groundFriction(xspeed,0.1,1.025);
               }
            }
            if(attack == true && attackFrame == "Dive" && Ice == false && Puddle == true)
            {
               if(StunTime == 0)
               {
                  xspeed = _root.groundFriction(xspeed,0.1,1.015);
               }
            }
            if(!(attack == true && attackFrame == "Dive") && Puddle == true)
            {
               if(StunTime == 0)
               {
                  xspeed = _root.groundFriction(xspeed,0.15,1.07);
               }
               else
               {
                  xspeed = _root.groundFriction(xspeed,0.075,1.035);
               }
            }
            if(attack == true && attackFrame == "Dive" && Ice == true)
            {
               if(StunTime == 0)
               {
                  xspeed = _root.groundFriction(xspeed,0.04,1.007);
               }
               else
               {
                  xspeed = _root.groundFriction(xspeed,0.02,1.0035);
               }
            }
            if(!(attack == true && attackFrame == "Dive") && Ice == true)
            {
               if(StunTime == 0)
               {
                  xspeed = _root.groundFriction(xspeed,0.05,1.01);
               }
               else
               {
                  xspeed = _root.groundFriction(xspeed,0.025,1.005);
               }
            }
         }
         if(Puddle == true)
         {
            _root.WaterAmount += 250;
         }
         _root.Power = _root.TotalPower;
         runcount--;
         if(runcount <= 0 || isNaN(runcount))
         {
            runcount = 0;
            runtap = "";
         }
         if(attack == false && _root.Controll == true && spinattackcounter <= 1)
         {
            if(runtap == "")
            {
               if(_root.KeyRIGHT())
               {
                  runcount = 9;
                  runtap = "R";
               }
               if(_root.KeyLEFT())
               {
                  runcount = 9;
                  runtap = "L";
               }
            }
            else
            {
               if(!_root.KeyRIGHT() && runtap == "R")
               {
                  runtap = "Right";
               }
               if(!_root.KeyLEFT() && runtap == "L")
               {
                  runtap = "Left";
               }
               if(_root.KeyRIGHT() && runtap == "Right")
               {
                  attack = true;
                  attackFrame = "Run";
               }
               if(_root.KeyLEFT() && runtap == "Left")
               {
                  attack = true;
                  attackFrame = "Run";
               }
            }
         }
         if(attack == false && _root.Controll == true)
         {
            if(_root.KeyRIGHT())
            {
               frame = 2;
               if(Ice == true)
               {
                  xspeed += _root.walkspeed / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed += _root.walkspeed / _root.PuddleAccel;
               }
               else
               {
                  xspeed += _root.walkspeed;
               }
               _xscale = -100;
            }
            if(_root.KeyLEFT())
            {
               frame = 2;
               if(Ice == true)
               {
                  xspeed -= _root.walkspeed / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed -= _root.walkspeed / _root.PuddleAccel;
               }
               else
               {
                  xspeed -= _root.walkspeed;
               }
               _xscale = 100;
            }
            if(_root.KeyTapRIGHT() && !_root.KeyRIGHT())
            {
               frame = 2;
               if(Ice == true)
               {
                  xspeed += _root.tapwalkspeed / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed += _root.tapwalkspeed / _root.PuddleAccel;
               }
               else
               {
                  xspeed += _root.tapwalkspeed;
               }
               _xscale = -100;
            }
            if(_root.KeyTapLEFT() && !_root.KeyLEFT())
            {
               frame = 2;
               if(Ice == true)
               {
                  xspeed -= _root.tapwalkspeed / _root.IceAccel;
               }
               else if(Puddle == true)
               {
                  xspeed -= _root.tapwalkspeed / _root.PuddleAccel;
               }
               else
               {
                  xspeed -= _root.tapwalkspeed;
               }
               _xscale = 100;
            }
            if(_root.KeyDOWN())
            {
               attack = true;
               attackFrame = "Dive";
            }
         }
         if(attack == true && _root.Course.Char.attackFrame !== "Land")
         {
            doublejump = 0;
         }
         if(attack == false)
         {
            if(_root.Metal == true && _root.JumpPress > 0 && _root.KeyUP() && _root.Controll == true && Squish == false && yspeed >= -3 && Water == true)
            {
               frame = 3;
               attack = false;
               lockon = false;
               yspeed = Math.min((- _root.jumpspeed) * 1.25,yspeed);
            }
            if(_root.KeyUP() && _root.JumpPress > 0 && _root.Controll == true && Squish == false && yspeed >= -3 && Water == false)
            {
               attack = false;
               lockon = false;
               if(Puddle == true)
               {
                  _root.Mariosound = new Sound(this);
                  _root.Mariosound.attachSound("step - Water");
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
               }
               if(doublejump == 0)
               {
                  yspeed = Math.min(- _root.jumpspeed,yspeed);
                  doublejumpcount = 9;
                  if(_root.CurrentPlayer == "Mario")
                  {
                     if(random(2) == 1)
                     {
                        soundj = "Mario Jump-1";
                     }
                     else
                     {
                        soundj = "Mario Jump-1b";
                     }
                  }
                  else if(_root.CurrentPlayer == "Luigi")
                  {
                     if(random(2) == 1)
                     {
                        soundj = "Luigi - Yah!";
                     }
                     else
                     {
                        soundj = "Luigi - Yuh!";
                     }
                  }
                  _root.Mariosound = new Sound(this);
                  _root.Mariosound.attachSound(soundj);
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
               }
               else if(doublejump == 1)
               {
                  yspeed = Math.min(- _root.jumpspeed - 2.5,yspeed);
                  doublejumpcount = 9;
                  _root.Mariosound = new Sound(this);
                  if(_root.CurrentPlayer == "Mario")
                  {
                     _root.Mariosound.attachSound("Mario Jump-2");
                  }
                  else
                  {
                     _root.Mariosound.attachSound("Luigi - Yoho!");
                  }
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
               }
               else if(Math.abs(xspeed) > 3)
               {
                  attack = true;
                  attackFrame = "frontflip";
                  yspeed = Math.min(- _root.jumpspeed - 5,yspeed);
                  _root.Mariosound = new Sound(this);
                  if(_root.CurrentPlayer == "Mario")
                  {
                     _root.Mariosound.attachSound("Mario Tripple Jump");
                  }
                  else
                  {
                     _root.Mariosound.attachSound("Luigi - Yahoo!");
                  }
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
                  if(_xscale < 0)
                  {
                     xspeed -= (xspeed - 15) / 5;
                  }
                  else
                  {
                     xspeed -= (xspeed + 15) / 5;
                  }
                  doublejumpcount = 0;
                  doublejump = -1;
               }
               if(doublejump >= 3)
               {
                  doublejump = 0;
               }
               doublejump++;
               frame = 3;
            }
         }
      }
      spinattackcount--;
      if(spinattackcount <= 0)
      {
         spinattackcount = 0;
         spincount = 0;
         spinattackcounter = 0;
      }
      if(_root.KeyRIGHT() && !_root.KeyLEFT())
      {
         if(spincount !== "R")
         {
            if(spinattackcounter < 1 || spinattackcount > 0)
            {
               spinattackcounter++;
               spinattackcount = 15 - spinattackcounter * 3;
            }
         }
         spincount = "R";
      }
      if(_root.KeyLEFT() && !_root.KeyRIGHT())
      {
         if(spincount !== "L")
         {
            if(spinattackcounter < 1 || spinattackcount > 0)
            {
               spinattackcounter++;
               spinattackcount = 11 - spinattackcounter;
            }
         }
         spincount = "L";
      }
      if((_root.KeySPIN() || spinattackcounter >= 4) && attack == false && yspeed > -3)
      {
         attackFrame = "Spin";
         attack = true;
         spinattackcount = 0;
         spincount = 0;
         spinattackcounter = 0;
      }
      if(spinattackcounter >= 3 && (attackFrame == "Spin" || attackFrame == "DiveSpin" || attackFrame == "SlingStar") && attack == true)
      {
         _root.Course.Char.charClip.speed *= 0.8;
         _root.Course.Char.charClip.speed += 15;
         spinattackcount = 0;
         spincount = 0;
         spinattackcounter = 0;
      }
      if(_root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "R")
      {
         _root.waterblast = true;
      }
      else
      {
         _root.waterblast = false;
      }
      if(Water == true && air == true && _root.Metal !== true)
      {
         fallcount = 0;
         hurtfallcount--;
         hurtfallcount = Math.max(hurtfallcount,0);
         frame = "swim1";
      }
      if(Water == true && air == true && _root.Metal == true)
      {
         if(yspeed < 0)
         {
            frame = 3;
         }
         else
         {
            frame = 4;
         }
      }
      if(swimup == true && attack == false && _root.Metal !== true && _root.Controll == true)
      {
         swimupcount++;
         frame = "swim2";
         if(!_root.KeyUP() && swimupcount > 5)
         {
            swimup = false;
         }
      }
      if(attack == false && _root.Controll == true && _root.playpause == false)
      {
         if((_root.KeyUP() || _root.KeyTapUP()) && (air == true || Water == true && _root.playpause == false))
         {
            yspeed -= _root.risespeed;
            if(Water == true)
            {
               yspeed += _root.risespeed;
               if(swimup == true)
               {
                  yspeed -= _root.risespeed * 2;
               }
               if(swimup == false && _root.Metal !== true)
               {
                  _root.Mariosound = new Sound(this);
                  _root.Mariosound.attachSound("sound - Water");
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
                  swimupcount = 0;
                  if(_root.KeyDOWN() || _root.KeyTapDOWN())
                  {
                     yspeed -= 4;
                  }
                  else
                  {
                     yspeed -= 6;
                  }
                  if(_root.KeyRIGHT())
                  {
                     xspeed += 2;
                  }
                  if(_root.KeyLEFT())
                  {
                     xpseed -= 2;
                  }
               }
               swimup = true;
            }
         }
         if(_root.KeyDOWN() || _root.KeyTapDOWN())
         {
            yspeed += _root.risespeed;
            if(Water == true)
            {
               yspeed += _root.risespeed * 2;
            }
         }
         if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
         {
            lockon = false;
            yspeed *= 0.8;
            yspeed -= (_root.Hoverspeed * 10 - yspeed) / 10 * (_root.Power / 175 + 0.75);
            _root.WaterAmount -= 7;
            _root.Power -= 1.5;
            xspeed = _root.groundFriction(xspeed,0.05,1.03);
         }
         if(_root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T")
         {
            _root.WaterAmount -= 5;
            if(air == false && Ice == false)
            {
               if(_xscale < 0)
               {
                  xspeed -= (xspeed - _root.turbospeed) / 30;
               }
               else
               {
                  xspeed -= (xspeed + _root.turbospeed) / 30;
               }
            }
            else if(_xscale < 0)
            {
               xspeed -= (xspeed - _root.turbospeed / 2) / 30;
            }
            else
            {
               xspeed -= (xspeed + _root.turbospeed / 2) / 30;
            }
            if(Water == true && _root.playpause == false)
            {
               yspeed -= _root.gravity / 3;
               if(_root.KeyUP() || _root.KeyTapUP())
               {
                  _rotation = _rotation + _xscale / 100 * 10;
                  yspeed -= _root.gravity / 1.5;
               }
               if(_root.KeyDOWN() || _root.KeyTapDOWN())
               {
                  _rotation = _rotation - _xscale / 100 * 10;
                  yspeed += _root.gravity / 1.5;
               }
            }
            if(Water == false && _root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true))
            {
               fallcount = 0;
               hurtfallcount--;
               hurtfallcount = Math.max(hurtfallcount,0);
               if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true) && _root.playpause == false)
               {
                  yspeed -= _root.gravity / 5;
                  _Y--;
               }
               waterrepeats = 0;
               while(waterrepeats < Math.abs(xspeed) / 7 + random(2))
               {
                  _root.Course.FrontGFX.attachMovie("Run-Splash" + (random(2) + 1),"Run-Splash" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X - _root.Course.FrontGFX._x + (waterrepeats - 1) / Math.round(xspeed) * 100,_y:_Y - _root.Course.FrontGFX._y});
                  waterrepeats++;
               }
               _root.WaterAmount += 50;
               yspeed = Math.min(0,yspeed);
               if(!_root.KeyRIGHT() && !_root.KeyLEFT())
               {
                  xspeed = _root.groundFriction(xspeed,0.1,1.05);
               }
               if(_xscale < 0)
               {
                  xspeed -= (xspeed - _root.turbospeed / 2) / 30;
               }
               else
               {
                  xspeed -= (xspeed + _root.turbospeed / 2) / 30;
               }
               frame = 2;
               if(_root.KeySPACE() && _root.KeyUP() && Squish == false)
               {
                  lockon = false;
                  yspeed = Math.min(yspeed,(- _root.jumpspeed) / 1.5);
                  frame = 3;
               }
            }
         }
      }
      if((frame == "swim1" || frame == "swim2") && Water == false)
      {
         if(air == true)
         {
            frame = 4;
         }
         else
         {
            frame = 1;
         }
      }
      if(attackFrame !== "Star")
      {
         if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _root.Course._y,true) && _root.playpause == false)
            {
               underwatercount++;
               if(underwatercount > _root.WaterDrownSpeed && _root.Metal !== true)
               {
                  _root.Course.FrontGFX.attachMovie("WaterBubble","Bubble" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X - _root.Course.FrontGFX._x,_y:_Y - _root.Course.FrontGFX._y - tall * 0.75});
                  _root.Mariosound = new Sound(this);
                  _root.Mariosound.attachSound("sound - Drain");
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
                  _root.WaterHP = _root.WaterHP - 1;
                  if(_root.WaterHP <= 0 && !(attack == true && attackFrame == "Drown"))
                  {
                     attack = true;
                     attackFrame = "Drown";
                  }
                  _root.WaterHP = Math.max(_root.WaterHP,0);
                  underwatercount = 0;
               }
            }
            else if(_root.playpause == false)
            {
               abovewatercount++;
               if(abovewatercount > _root.WaterHealSpeed)
               {
                  abovewatercount = 0;
                  _root.WaterHP += 1;
                  _root.WaterHP = Math.min(_root.WaterHP,8);
               }
            }
         }
      }
      if(Water == true)
      {
         _root.WaterAmount += 500;
         _root.Power += 3;
         yspeed = _root.groundFriction(yspeed,0.05,1.1);
         xspeed = _root.groundFriction(xspeed,0.05,1.05);
      }
      else
      {
         underwatercount = 0;
         abovewatercount++;
         if(abovewatercount > _root.WaterHealSpeed)
         {
            abovewatercount = 0;
            _root.WaterHP += 1;
            _root.WaterHP = Math.min(_root.WaterHP,8);
         }
      }
      if(attack == true && (attackFrame == "Dive" || attackFrame == "frontflip"))
      {
         if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "S")
         {
            _root.WaterAmount -= 5;
            _root.Power -= 1;
         }
         if(_root.KeySPACE() && _root.Power > 0 && _root.WaterAmount > 0 && _root.Fluddpow == "H")
         {
            _root.WaterAmount -= 7;
            _root.Power -= 1.5;
            yspeed *= 0.98;
            xspeed *= 0.97;
            if(air == true)
            {
               yspeed -= Math.sin(_root.angleToRadians(_root.DiveRot)) * _root.Hoverspeed;
               if(_xscale > 0)
               {
                  xspeed -= Math.cos(_root.angleToRadians(_root.DiveRot)) * _root.Hoverspeed / 2;
               }
               else
               {
                  xspeed += Math.cos(_root.angleToRadians(_root.DiveRot)) * _root.Hoverspeed / 2;
               }
            }
            else if(_xscale > 0)
            {
               xspeed -= Math.cos(_root.angleToRadians(_root.DiveRot)) * _root.Hoverspeed;
            }
            else
            {
               xspeed += Math.cos(_root.angleToRadians(_root.DiveRot)) * _root.Hoverspeed;
            }
         }
         if(_root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T")
         {
            _root.WaterAmount -= 10;
            yspeed -= Math.sin(_root.angleToRadians(_root.DiveRot - 90)) / 2;
            if(_xscale > 0)
            {
               xspeed -= Math.cos(_root.angleToRadians(_root.DiveRot - 90)) / 2;
            }
            else
            {
               xspeed += Math.cos(_root.angleToRadians(_root.DiveRot - 90)) / 2;
            }
         }
      }
      if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true) && _root.CharHP >= 0 && !((attackFrame == "Fall" || attackFrame == "Respawn" || attackFrame == "Faint" || attackFrame == "Drown" || attackFrame == "Pause" || attackFrame == "none" || attackFrame == "Squish") && attack == true || _root.Metal == true || _root.Invincible == true) && _root.playpause == false && _root.Course.Char.yspeed > -3)
      {
         _root.Course.Char.blink = 0;
         _root.MarioHit(3,20,0,0,20,"none");
         if(_root.CharHP >= 0)
         {
            _root.Course.Char.yspeed = Math.min(-16,_root.Course.Char.yspeed);
            attack = true;
            attackFrame = "Lava";
         }
         else
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.attackFrame = "Faint";
         }
         while(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 1) * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
         }
      }
      _root.AntiSquish = Math.max(_root.AntiSquish,0);
      if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true) && air == false && !((attackFrame == "Fall" || attackFrame == "Respawn" || attackFrame == "Faint" || attackFrame == "Drown" || attackFrame == "Pause") && attack == true) && (!_root.Course.Platforms.invisground.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) && (!_root.Course.Platforms.invisground.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) && _root.playpause == false && _root.AntiSquish-- <= 0)
      {
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 100) * _root.coursescale / 100 + _root.Course._y,true))
         {
            _yscale--;
         }
         if(_yscale < 70 && _root.CharHP >= 1 && !(attack == true && attackFrame == "Squish"))
         {
            _root.CharHP -= 2;
            _root.hpcoincount = 0;
            xspeed = 0;
            yspeed = 0;
            attack = true;
            attackFrame = "Squish";
         }
         if(attack == true && attackFrame == "Squish")
         {
            _yscale = 100;
         }
         Squish = true;
      }
      else if(!_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _root.Course._y,true))
      {
         Squish = false;
         _yscale = _yscale + 5;
         _yscale = Math.min(_yscale,100);
      }
      if(_root.playpause == true && Water == true)
      {
         yspeed = 0;
      }
      if(!_root.KeySPACE() && _root.Fluddpow !== "H")
      {
         _root.Power += 1;
      }
      repeats = Math.ceil((Math.abs(yspeed) + Math.abs(xspeed)) / 5);
      if(isNaN(repeats))
      {
         repeats = 0;
      }
      repeats = Math.min(repeats,50);
      repeats = Math.max(repeats,1);
      i = 1;
      while(i <= repeats)
      {
         if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 4) * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(Water == false)
            {
               if(yspeed > 7)
               {
                  yspeed *= 0.7;
                  _root.Course.FrontGFX.attachMovie("Med-Splash","Med-Splash" + _root.Course.FrontGFX.getNextHighestDepth(),_root.Course.FrontGFX.getNextHighestDepth(),{_x:_X - _root.Course.FrontGFX._x,_y:_Y - _root.Course.FrontGFX._y});
                  _root.Mariosound = new Sound(this);
                  _root.Mariosound.attachSound("sound - Splash");
                  _root.Mariosound.start(0,1);
                  _root.Mariosound.setVolume(_root.MarioVolume);
               }
               yspeed *= 0.9;
            }
            Water = true;
            doublejump = 0;
         }
         else
         {
            Water = false;
         }
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _root.Course._y,true))
         {
            air = false;
            if(yspeed < 16)
            {
               landtime = Math.round(fallcount / 8) - 2;
               if(_root.KeyUP())
               {
                  landtime -= 3;
                  if((_root.KeyRIGHT() || _root.KeyLEFT()) && landtime >= 1)
                  {
                     landtime -= 2;
                     landtime = Math.max(landtime,1);
                  }
               }
               landtime -= Math.min(Math.abs(xspeed / 1.5),3);
               if(yspeed > 3 && landtime > 0 && attack == false)
               {
                  landtime = Math.max(landtime,1);
                  landtime = Math.min(landtime,3);
                  attack = true;
                  attackFrame = "land";
               }
               if(yspeed > 3 && landtime < 1 && attack == false && _root.KeyUP())
               {
                  landtime = 1;
                  attack = true;
                  attackFrame = "land";
               }
            }
            fallcount = 0;
            hurtfallcount--;
            hurtfallcount = Math.max(hurtfallcount,0);
            _root.FallSquish();
            yspeed = Math.min(0,yspeed);
         }
         else
         {
            air = true;
         }
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _root.Course._y,true) && !(Squish == true && (_yscale < 70 || attack == true && attackFrame == "Squish")) && (!_root.Course.Platforms.invisground.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _root.Course._y,true) && yspeed >= -3)
         {
            _Y--;
            if(!(attack == true && attackFrame == "Dive"))
            {
               xspeed *= 0.98;
            }
            else
            {
               xspeed *= 0.997;
            }
            _root.FallSquish();
            yspeed = Math.min(0,yspeed);
         }
         if(Water == true)
         {
            lockon = false;
         }
         if(lockon == true)
         {
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
            {
               _Y++;
               if(!(attack == true && attackFrame == "Dive"))
               {
                  xspeed *= 0.98;
               }
               else
               {
                  xspeed *= 0.998;
               }
               frame = 2;
            }
            while(_root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _root.Course._y,true) && yspeed >= -3)
            {
               _Y++;
               if(!(attack == true && attackFrame == "Dive"))
               {
                  xspeed *= 0.98;
               }
               else
               {
                  xspeed *= 0.998;
               }
               frame = 2;
            }
         }
         if(!(Squish == true && (_yscale < 70 || attack == true && attackFrame == "Squish")))
         {
            while(_root.Course.Platforms.hitTest((_X + wide - 1) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest((_X + wide - 1) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
            {
               _X--;
            }
            while(_root.Course.Platforms.hitTest((_X - wide + 1) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest((_X - wide + 1) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
            {
               _X++;
            }
         }
         if(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest((_X + wide) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
         {
            if(Math.abs(xspeed) > 11 && (attack == false || attackFrame == "Run" || attackFrame == "Flying") || attack == true && (attackFrame == "Run" || attackFrame == "Flying") && Math.abs(xspeed) > 5 || attack == true && attackFrame == "Lava" && Math.abs(xspeed) > 4 && air == false && Water == false || attack == true && attackFrame == "Flying" && Math.abs(xspeed) > 3 && air == false && Water == false)
            {
               _root.HitWall();
            }
            else if(_root.KeyRIGHT())
            {
               xspeed = Math.min(xspeed,(- xspeed) * _root.bounce);
               xspeed = _root.groundFriction(xspeed,3,1);
            }
            else
            {
               xspeed = Math.min(xspeed - 1,(- xspeed) * _root.bounce);
            }
         }
         if(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest((_X - wide) * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 200) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
         {
            if(Math.abs(xspeed) > 11 && (attack == false || attackFrame == "Run" || attackFrame == "Flying") || attack == true && (attackFrame == "Run" || attackFrame == "Flying") && Math.abs(xspeed) > 5 || attack == true && attackFrame == "Lava" && Math.abs(xspeed) > 4 && air == false && Water == false || attack == true && attackFrame == "Flying" && Math.abs(xspeed) > 3 && air == false && Water == false)
            {
               _root.HitWall();
            }
            else if(_root.KeyLEFT())
            {
               xspeed = Math.min(xspeed,(- xspeed) * _root.bounce);
               xspeed = _root.groundFriction(xspeed,3,1);
            }
            else
            {
               xspeed = Math.max(xspeed + 1,(- xspeed) * _root.bounce);
            }
         }
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall + 1) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall - 1) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
         {
            _root.FallSquish();
            yspeed = Math.max(yspeed,-3);
         }
         if(Squish == false || _root.Course.Char.yspeed < -1)
         {
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall * _yscale / 100) * _root.coursescale / 100 + _root.Course._y,true) && (!_root.Course.Platforms.invisground.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _root.Course._y,true) || _root.Invisible !== true))
            {
               _Y++;
            }
         }
         if(_root.Course.Edge.hitTest(_X * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _root.Course._y,true) && !((attackFrame == "Fall" || attackFrame == "Respawn" || attackFrame == "Faint" || attackFrame == "Drown" || attackFrame == "Pause") && attack == true) && _root.playpause == false)
         {
            attack = true;
            attackFrame = "Fall";
         }
         _X = _X + xspeed / repeats;
         _Y = _Y + yspeed / repeats;
         i++;
      }
      if(air == false)
      {
         _root.CharRotationCode();
      }
      else
      {
         icedive = false;
         if(attack == false || attackFrame !== "SlingStar")
         {
            _rotation = _root.groundFriction(_rotation,3,1.3);
         }
      }
      if(attack == false || attackFrame !== "SlingStar")
      {
         _rotation = Math.max(_rotation,-30);
         _rotation = Math.min(_rotation,30);
      }
      if(_root.CharHP <= 0 && !((attackFrame == "Fall" || attackFrame == "Respawn" || attackFrame == "Faint" || attackFrame == "Drown" || attackFrame == "Pause" || attackFrame == "none" || attackFrame == "hurt1" || attackFrame == "hurt2" || attackFrame == "hurt3") && attack == true) && _root.playpause == false)
      {
         attack = true;
         attackFrame = "Faint";
         _yscale = 100;
      }
      if(_root.Invisible == true)
      {
         if(_root.Course.Platforms.invisground.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - _root.Course.Char.tall / 2) * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Platforms.invisground.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - _root.Course.Char.tall) * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Platforms.invisground.hitTest((_root.Course.Char._x + _root.Course.Char.wide) * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - _root.Course.Char.tall / 2) * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Platforms.invisground.hitTest((_root.Course.Char._x - _root.Course.Char.wide) * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - _root.Course.Char.tall / 2) * _root.coursescale / 100 + _root.Course._y,true))
         {
            _root.PowerTimer = Math.max(_root.PowerTimer,2);
            if(_root.PowerTimer < 32)
            {
               _root.PowerTimer += 4;
            }
         }
      }
      if(_root.CurrentPlayer == "Mario")
      {
         if(_root.PowerTimer > 160 || Math.round(_root.PowerTimer / Math.round(_root.PowerTimer / 32)) % 2 == 0)
         {
            if(_root.Metal == true && _root.Invincible !== true)
            {
               _root.Course.Char.gotoAndStop(2);
            }
            if(_root.Invisible == true)
            {
               if(_root.Metal !== true && _root.Invincible !== true)
               {
                  _root.Course.Char.gotoAndStop(1);
               }
               _root.Course.Char._alpha = 50;
            }
            if(_root.Invincible == true)
            {
               if(_root.Metal !== true)
               {
                  _root.Course.Char.gotoAndStop(3);
               }
               else
               {
                  _root.Course.Char.gotoAndStop(4);
               }
            }
         }
         else
         {
            _root.Course.Char._alpha = 100;
            _root.Course.Char.gotoAndStop(1);
         }
         _root.jumpspeed = _root.Mariojumpspeed;
      }
      if(_root.CurrentPlayer == "Luigi")
      {
         if(_root.PowerTimer > 160 || Math.round(_root.PowerTimer / Math.round(_root.PowerTimer / 32)) % 2 == 0)
         {
            if(_root.Metal == true)
            {
               _root.Course.Char.gotoAndStop(6);
            }
            if(_root.Invisible == true)
            {
               if(_root.Metal !== true && _root.Invincible !== true)
               {
                  _root.Course.Char.gotoAndStop(5);
               }
               _root.Course.Char._alpha = 50;
            }
            if(_root.Invincible == true)
            {
               if(_root.Metal !== true)
               {
                  _root.Course.Char.gotoAndStop(7);
               }
               else
               {
                  _root.Course.Char.gotoAndStop(8);
               }
            }
         }
         else
         {
            _root.Course.Char._alpha = 100;
            _root.Course.Char.gotoAndStop(5);
         }
         _root.jumpspeed = _root.Luigijumpspeed;
      }
      if(_root.Metal !== true && Puddle == false && _root.changestep-- < 0)
      {
         _root.StepSound = _root.defaultstep;
      }
      else if(_root.Metal == true)
      {
         _root.StepSound = "step-metalcap";
      }
      else if(Puddle == true)
      {
         _root.StepSound = "step - Water";
      }
      if(_root.Metal == true || _root.Invisible == true || _root.Invincible == true || _root.WingCap == true)
      {
         if(_root.Invincible == true)
         {
            _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X - _root.Course.BackGFX._x,_y:_Y - _root.Course.BackGFX._y - 20,_yscale:100,_xscale:60,time:3});
         }
         _root.PowerTimer -= 1;
         if(_root.PowerTimer <= 0)
         {
            _root.Metal = false;
            _root.Invisible = false;
            _root.Invincible = false;
            _root.WingCap = false;
            _root.PowerTimer = 0;
            _root.PlayMusicAndIntro();
         }
      }
      if((attackFrame == "Fall" || attackFrame == "Respawn" || attackFrame == "Faint" || attackFrame == "Drown" || attackFrame == "Pause" || attackFrame == "Squish" || attackFrame == "Sign" || attackFrame == "Item" || attackFrame == "Key" || attackFrame == "Door" || attackFrame == "FarDoor" || attackFrame == "FrontDoor" || attackFrame == "BigDoor" || attackFrame == "Painting" || attackFrame == "DiveSpin" || attackFrame == "grabbed" || attackFrame == "WarpPipe" || attackFrame == "Lava" || attackFrame == "hurt1" || attackFrame == "hurt2" || attackFrame == "hurt3" || attackFrame == "land" || attackFrame == "EJump" || attackFrame == "bosscinema" || attackFrame == "Talk" || attackFrame == "UnlockDoor" || attackFrame == "Run" || attackFrame == "SlingStar" || attackFrame == "Ground" || attackFrame == "Flying") && attack == true || _root.playpause == true || attackFrame == "none")
      {
         if(attackFrame !== "Lava" && attackFrame !== "hurt1" && attackFrame !== "hurt2" && attackFrame !== "hurt3" && attackFrame !== "land" && attackFrame !== "EJump" && attackFrame !== "Run" && attackFrame !== "SlingStar" && attackFrame !== "Ground" && attackFrame !== "Flying" && attack == true || _root.playpause == true || attackFrame == "none")
         {
            yspeed = - _root.gravity;
            if(Water == true)
            {
               yspeed = 0;
            }
            xspeed = 0;
         }
         if(_root.playpause == false || attackFrame == "bosscinema" || attackFrame == "Talk" || attackFrame == "UnlockDoor")
         {
            charClip.gotoAndStop(attackFrame);
         }
      }
      else if(attack == false)
      {
         charClip.gotoAndStop(_root.Fluddpow + frame);
      }
      else
      {
         charClip.gotoAndStop(_root.Fluddpow + attackFrame);
      }
      _root.CharXscale = _root.Course.Char._xscale;
   }
};
