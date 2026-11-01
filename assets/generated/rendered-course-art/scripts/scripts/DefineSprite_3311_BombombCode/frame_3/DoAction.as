stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600 || battleArena == true)
      {
         _yscale = scale * 100;
         _xscale = scale * 100;
         eye._yscale = 1 / scale * 100;
         eye._xscale = 1 / scale * 40;
         if(hurt == true)
         {
            xspeed = 0;
            yspeed = 0;
            tumble = false;
            clip.frame._rotation = tumblerot;
         }
         distancevolume = Math.sqrt(Math.pow(_root.currentCamX - (_X + _parent._x),2) + Math.pow(_root.currentCamY - (_Y + _parent._y),2));
         distancevolume /= 100;
         distancevolume *= 15;
         distancevolume = Math.min(100,distancevolume);
         distancevolume = Math.max(0,distancevolume);
         distancevolume -= 100;
         distancevolume *= -1;
         if(distancevolume >= 1)
         {
            soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
            panX = _root.currentCamX - (_X + _parent._x);
            panX /= -2;
            panX = Math.min(100,panX);
            panX = Math.max(-100,panX);
            soundclip.setPan(panX);
         }
         if((frame == "explode" || frame == "explode2") && coinappear == true)
         {
            if(_root["coindata" + _root.playingcourse][coinnum] !== 1 || battleArena == true)
            {
               if(battleArena !== true)
               {
                  _root["coindata" + _root.playingcourse][coinnum] = 1;
               }
               else
               {
                  _root.Course.BackGFX.defeatedcount++;
               }
               _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X,_y:_Y - 12,bounce:true});
               coinappear = false;
            }
         }
         if(frame !== "explode" && frame !== "explode2")
         {
            if(_root.playpause == false)
            {
               if(this.eye.hitTest(_root.Course.Char.charClip.body))
               {
                  if(tumble == false)
                  {
                     if(seeing == false)
                     {
                        see = true;
                     }
                     seeing = true;
                  }
               }
               else
               {
                  seeing = false;
                  see = false;
               }
            }
            else
            {
               seeing = false;
               see = false;
            }
            if(this.body.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 1)
            {
               tumble = true;
               yspeed -= Math.abs((_root.Course.Char.xspeed + 1) / 5) / scale;
               xspeed += (_root.Course.Char.xspeed - xspeed / 1.5) / scale;
               _Y = _Y - 3;
               if(Math.abs(xspeed / (scale / 2)) > 17)
               {
                  powerhit = true;
               }
            }
            if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               tumble = true;
               yspeed -= 1.4 / scale;
               xspeed += (Math.random() * 4 - 2 - xspeed / 1.5) / scale;
               _Y = _Y - 3;
               if(Math.abs(yspeed / (scale / 2)) > 5)
               {
                  powerhit = true;
               }
            }
            else
            {
               if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  tumble = true;
                  yspeed -= 1.4 / scale;
                  xspeed += Math.max((12 - xspeed / 1.5) / scale,0);
                  _Y = _Y - 3;
                  if(Math.abs(xspeed / (scale / 2)) > 10)
                  {
                     powerhit = true;
                  }
               }
               if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  tumble = true;
                  yspeed -= 1.4 / scale;
                  xspeed += Math.min((-12 - xspeed / 1.5) / scale,0);
                  _Y = _Y - 3;
                  if(Math.abs(xspeed / (scale / 2)) > 10)
                  {
                     powerhit = true;
                  }
               }
            }
            if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water) || this.head.hitTest(_root.Course.Char.charClip.water) || this.head.hitTest(_root.Course.Char.charClip.frame.water))
            {
               tumble = true;
               yspeed *= 0.9;
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
               if(Math.abs(xspeed / (scale / 2)) > 9)
               {
                  powerhit = true;
               }
               _Y = _Y - 3;
            }
            if(this.body.hitTest(_root.Course.Char.charClip.body) && (Math.abs(_root.Course.Char.xspeed) >= 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T"))
            {
               tumble = true;
               yspeed -= Math.abs((_root.Course.Char.xspeed + 1) / 5) / scale;
               xspeed += (_root.Course.Char.xspeed - xspeed / 1.5) / scale * 0.8;
               _Y = _Y - 3;
               if(Math.abs(xspeed / (scale / 2)) > 20)
               {
                  powerhit = true;
               }
            }
            if(this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attack == false || !(_root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3)) && tumble == false && !(Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T"))
            {
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(0,0,0,3,0,"none");
                  xspeed -= Math.max(_root.Course.Char.xspeed / 3,0);
               }
               else
               {
                  _root.MarioHit(0,0,0,-3,0,"none");
                  xspeed -= Math.min(_root.Course.Char.xspeed / 3,0);
               }
            }
            if(_root.Course.Water.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               hurt = true;
               tumblerot = 0;
            }
            if(!_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y = _Y + yspeed;
               yspeed += 1;
               yspeed *= 0.98;
               air = true;
            }
            if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               if(air == true)
               {
                  jump = false;
               }
               air = false;
               if(tumble == false)
               {
                  yspeed = Math.min(0,yspeed);
               }
               else
               {
                  if(Math.abs(yspeed) + Math.abs(xspeed / 2) > 11)
                  {
                     tumble = false;
                     if(powerhit == true)
                     {
                        tumblerot = 0;
                        hurt = true;
                     }
                  }
                  if(yspeed > 3)
                  {
                     yspeed = Math.min(0,- yspeed + 2);
                     xspeed = _root.groundFriction(xspeed,2,1.2);
                  }
                  else
                  {
                     xspeed = _root.groundFriction(xspeed,0.5,1.1);
                     yspeed = 0;
                  }
                  if(Math.abs(xspeed) < 2)
                  {
                     tumble = false;
                     if(powerhit == true)
                     {
                        tumblerot = 0;
                        hurt = true;
                     }
                  }
               }
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y--;
               if(powerhit == true && Math.abs(xspeed) > 3)
               {
                  hurt = true;
                  tumblerot = 0;
               }
            }
            while(_root.Course.Platforms.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _X--;
               movedirection = "left";
               randomnum = 0;
               if(tumble == true)
               {
                  if(xspeed < 7)
                  {
                     xspeed = Math.max(xspeed * 0.7,(- xspeed) * 0.7);
                  }
                  else if(xspeed > 0 && powerhit == true)
                  {
                     hurt = true;
                     tumblerot = -90;
                  }
               }
            }
            while(_root.Course.Platforms.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _X++;
               movedirection = "right";
               randomnum = 0;
               if(tumble == true)
               {
                  if(xspeed > 7)
                  {
                     xspeed = Math.min(xspeed * 0.7,(- xspeed) * 0.7);
                  }
                  else if(xspeed < 0 && powerhit == true)
                  {
                     hurt = true;
                     tumblerot = 90;
                  }
               }
            }
            while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
            {
               _Y++;
            }
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y--;
            }
            if(hurt == true)
            {
               if(frame !== "explode" && frame !== "explode2")
               {
                  distancevolume = Math.sqrt(Math.pow(_root.currentCamX - (_X + _parent._x),2) + Math.pow(_root.currentCamY - (_Y + _parent._y),2));
                  distancevolume /= 100;
                  distancevolume *= 20;
                  distancevolume = Math.min(100,distancevolume);
                  distancevolume = Math.max(0,distancevolume);
                  distancevolume -= 100;
                  distancevolume *= -1;
                  if(distancevolume >= 1)
                  {
                     soundclip.stop();
                     soundclip = new Sound(this);
                     soundclip.attachSound("Explosion");
                     soundclip.start(0,1);
                     soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
                     soundclip.setPan(panX);
                  }
                  _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y - 25,_yscale:50,_xscale:35,time:6});
               }
               if(air == true)
               {
                  frame = "explode2";
               }
               else
               {
                  frame = "explode";
               }
            }
            if(seeing == true && tumble == false && frame !== "explode" && frame !== "explode2")
            {
               if(_root.Course.Char._x > _X + _parent._x && Math.abs(_root.Course.Char._x - (_X + _parent._x)) > 20)
               {
                  Moveright();
               }
               else if(_root.Course.Char._x < _X + _parent._x && Math.abs(_root.Course.Char._x - (_X + _parent._x)) > 20)
               {
                  Moveleft();
               }
               else if(air == false && frame !== "explode" && frame !== "explode2" && tumble == false)
               {
               }
            }
            if(seeing == false && tumble == false && frame !== "explode" && frame !== "explode2")
            {
               randomnum++;
               if(movedirection == "right")
               {
                  Moveright();
               }
               if(movedirection == "left")
               {
                  Moveleft();
               }
               if(randomnum > WalkDistance)
               {
                  if(movedirection == "right")
                  {
                     movedirection = "left";
                  }
                  else
                  {
                     movedirection = "right";
                  }
                  randomnum = 0;
               }
            }
            if(see == true)
            {
               see = false;
               if(exploding == false)
               {
                  soundclip = new Sound(this);
                  soundclip.attachSound("fuse-lite");
                  soundclip.start(0,1);
                  soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
                  soundclip.setPan(panX);
               }
               exploding = true;
            }
            if(air == false && frame !== "explode" && frame !== "explode2" && tumble == false && Math.abs(xspeed) > 0.3 && (_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
            {
               if(xspeed > 0)
               {
                  frame = "Walkright";
               }
               else
               {
                  frame = "Walkleft";
               }
            }
            else if(air == false && frame !== "explode" && frame !== "explode2" && tumble == false)
            {
            }
            if(air == true && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               air == false;
               if(frame !== "explode" && tumble == false && Math.abs(xspeed) > 0.3 && (_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
               {
                  if(xspeed > 0)
                  {
                     frame = "Walkright";
                  }
                  else
                  {
                     frame = "Walkleft";
                  }
               }
               else if(air == false && frame !== "explode" && frame !== "explode2" && tumble == false)
               {
               }
            }
            if(tumble == true && hurt == false)
            {
               tumblerot += xspeed * 3;
               frame = "tumble";
            }
            if(yspeed > 7)
            {
               yspeed *= 0.9;
            }
            if(Math.abs(clip.frame._rotation) > 0 && frame !== "tumble")
            {
               clip.frame._rotation *= 0.8;
               tumblerot *= 0.8;
               if(Math.abs(clip.frame._rotation) < 10)
               {
                  clip.frame._rotation = 0;
                  tumblerot = 0;
               }
            }
            yspeed = Math.max(yspeed,-12);
            yspeed = Math.min(yspeed,9);
            xspeed = Math.max(xspeed,-10);
            xspeed = Math.min(xspeed,10);
            _X = _X + xspeed;
            if(tumble == false)
            {
               xspeed = _root.groundFriction(xspeed,0.3,1.1);
            }
            else
            {
               xspeed = _root.groundFriction(xspeed,0.05,1.01);
            }
            if(tumble == false && Math.abs(tumblerot) > 0)
            {
               tumblerot = _root.groundFriction(tumblerot,5,1.3);
               frame = "tumble";
            }
            if(frame !== "tumble")
            {
               tumblerot = 0;
            }
            soundclip.onSoundComplete = function()
            {
               if(exploding == true && frame !== "explode" && frame !== "explode2")
               {
                  soundclip.attachSound("fuse-loop");
                  soundclip.start(0,1);
                  soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
                  soundclip.setPan(panX);
               }
            };
            if(exploding == true)
            {
               explodetimer++;
               myColor = new Color(this);
               colorTrans = new Object();
               powerblink = Math.sin(_root.angleToRadians(Math.pow(explodetimer,3) / 700)) * 25 + 25 + explodetimer / 6;
               colorTrans = {ra:100,rb:Math.pow(explodetimer,2) / 75 + powerblink,ga:(- Math.pow(explodetimer,2)) / 100 + 100 + powerblink,gb:0,ba:(- Math.pow(explodetimer,2)) / 100 + 100 + powerblink,bb:0,aa:100,ab:0};
               myColor.setTransform(colorTrans);
               if(explodetimer >= 96)
               {
                  soundclip.stop();
                  if(frame !== "explode" && frame !== "explode2")
                  {
                     soundclip.stop();
                     soundclip = new Sound(this);
                     soundclip.attachSound("Explosion");
                     soundclip.start(0,1);
                     soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
                     soundclip.setPan(panX);
                     _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y - 15,_yscale:30,_xscale:30,time:6});
                  }
                  if(air == true)
                  {
                     frame = "explode2";
                  }
                  else
                  {
                     frame = "explode";
                  }
                  myColor = new Color(this);
                  colorTrans = new Object();
                  colorTrans = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
                  myColor.setTransform(colorTrans);
               }
            }
            clip.frame._rotation = tumblerot;
            clip.gotoAndStop(frame);
            if(frame == "explode" || frame == "explode2")
            {
               myColor = new Color(this);
               colorTrans = new Object();
               colorTrans = {ra:100,rb:0,ga:100,gb:0,ba:100,bb:0,aa:100,ab:0};
               myColor.setTransform(colorTrans);
            }
         }
         if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            frame = "explode";
         }
         if(this.clip.frame.explode.hitTest(_root.Course.Char.charClip.body))
         {
            if(_root.Course.Char._x > _X + _parent._x)
            {
               _root.MarioHit(2,90,-7,5,10,"Mario-Squish",true);
            }
            else
            {
               _root.MarioHit(2,90,-7,-5,10,"Mario-Squish",true);
            }
         }
      }
      clip.gotoAndStop(frame);
      if(frame == "explode" || frame == "explode2")
      {
         deadcount++;
      }
      if((deadcount >= 30 || Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600) && battleArena !== true)
      {
         _Y = saveY;
         _X = saveX;
         restartvars();
      }
   }
};
