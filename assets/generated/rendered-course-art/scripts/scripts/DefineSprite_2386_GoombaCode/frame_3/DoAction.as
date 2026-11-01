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
         eye._xscale = 1 / scale * 100;
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
         if(frame !== "Squish")
         {
            if(_root.playpause == false)
            {
               if(this.eye.hitTest(_root.Course.Char.charClip.body) && tumble == false)
               {
                  if(seeing == false)
                  {
                     see = true;
                     if(distancevolume >= 1)
                     {
                        soundclip = new Sound(this);
                        soundclip.attachSound("Goomba-Attention");
                        soundclip.start(0,1);
                        soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
                        soundclip.setPan(panX);
                     }
                  }
                  seeing = true;
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
               xspeed += (_root.Course.Char.xspeed - xspeed / 1.5) / scale;
               _Y = _Y - 3;
               if(Math.abs(xspeed / (scale / 2)) > 25)
               {
                  powerhit = true;
               }
            }
            if(this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attack == false || !(_root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3)) && tumble == false && !(Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T"))
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
            if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0)
            {
               _root.Course.Char.yspeed = 0;
               hurt = true;
               tumblerot = 0;
               bounce = true;
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
            if(_root.Course.Lava.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _root.Course.Char.yspeed = 0;
               hurt = true;
               tumblerot = 0;
               bounce = false;
            }
            if(hurt == true)
            {
               if(frame !== "squish")
               {
                  if(distancevolume >= 1)
                  {
                     soundclip = new Sound(this);
                     soundclip.attachSound("Goomba-Stomped");
                     soundclip.start(0,1);
                     soundclip.setVolume(_root.MarioVolume / 100 * distancevolume);
                     soundclip.setPan(panX);
                  }
               }
               frame = "Squish";
            }
            if(air == true && frame !== "Squish")
            {
               frame = "jump";
            }
            if(seeing == true && tumble == false && frame !== "Squish")
            {
               if(_root.Course.Char._x > _X + _parent._x && Math.abs(_root.Course.Char._x - (_X + _parent._x)) > 20)
               {
                  Moveright();
               }
               else if(_root.Course.Char._x < _X + _parent._x && Math.abs(_root.Course.Char._x - (_X + _parent._x)) > 20)
               {
                  Moveleft();
               }
               else if(air == false && frame !== "Squish" && tumble == false)
               {
                  frame = "Idle";
               }
            }
            if(seeing == false && tumble == false && frame !== "Squish")
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
               jump = true;
               see = false;
            }
            if(jump == true && air == false)
            {
               yspeed = -5;
               _Y = _Y - 3;
               jump = false;
               frame = "jump";
            }
            if(air == false && frame !== "Squish" && tumble == false && Math.abs(xspeed) > 0.3 && (_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
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
            else if(air == false && frame !== "Squish" && tumble == false)
            {
               frame = "Idle";
            }
            if(air == true && _root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               air == false;
               if(frame !== "Squish" && tumble == false && Math.abs(xspeed) > 0.3 && (_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
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
               else if(air == false && frame !== "Squish" && tumble == false)
               {
                  frame = "Idle";
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
            yspeed = Math.max(yspeed,-10);
            yspeed = Math.min(yspeed,10);
            xspeed = Math.max(xspeed,-15);
            xspeed = Math.min(xspeed,15);
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
            clip.frame._rotation = - tumblerot;
            clip.gotoAndStop(frame);
            if(frame == "tumble" && powerhit == false)
            {
               clip.frame.gotoAndStop(2);
            }
            else if(frame == "tumble")
            {
               clip.frame.gotoAndStop(1);
            }
         }
      }
      else
      {
         if(frame == "Squish")
         {
            deadcount++;
         }
         if((deadcount > 30 || Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600) && battleArena !== true)
         {
            _Y = saveY;
            _X = saveX;
            restartvars();
         }
      }
   }
};
