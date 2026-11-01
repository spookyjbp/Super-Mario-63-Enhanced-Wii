stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600 || battleArena == true)
   {
      if(frame !== "spin" && frame !== "gone")
      {
         distance = Math.sqrt(Math.pow(_root.Course.Char._y - (_Y + _parent._y),2) + Math.pow(_root.Course.Char._x - (_X + _parent._x),2));
         if(stopchasing > 0)
         {
            stopchasing--;
         }
         if(HitTest("Water",_X,_Y))
         {
            if(lastframewater == false)
            {
               frame = "swim";
            }
            lastframewater = true;
            if(chase == false)
            {
               _rotation = _rotation * 0.7;
               count++;
               if(count > turncount)
               {
                  count = 0;
                  frame = "turn";
                  _xscale = _xscale * -1;
               }
               if(_xscale < 0)
               {
                  xspeed += (idlespeed - xspeed) / 7;
               }
               else
               {
                  xspeed += (- idlespeed - xspeed) / 7;
               }
               if(distance < 80 * scale && hungry == true && stopchasing <= 0 && _root.playpause == false)
               {
                  chase = true;
               }
            }
            else
            {
               frame = "chase";
               rot = _root.radiansToAngle(Math.atan2(_root.Course.Char._y - (_Y + _parent._y),_root.Course.Char._x - (_X + _parent._x))) - 180;
               if(_root.Course.Char._x - (_X + _parent._x) > 0)
               {
                  if(_xscale > 0)
                  {
                     _rotation = _rotation + 180;
                     _xscale = _xscale * -1;
                  }
                  rot += 180;
               }
               else if(_xscale < 0)
               {
                  _rotation = _rotation + 180;
                  _xscale = _xscale * -1;
               }
               while(rot > 180)
               {
                  rot -= 360;
               }
               while(rot < -180)
               {
                  rot += 360;
               }
               rotspeed = (- (_rotation - rot)) / 3;
               _rotation = _rotation + rotspeed;
               if(_xscale > 0)
               {
                  xspeed -= Math.cos(_root.angleToRadians(_rotation)) * chasespeed;
                  yspeed -= Math.sin(_root.angleToRadians(_rotation)) * chasespeed;
               }
               else
               {
                  xspeed += Math.cos(_root.angleToRadians(_rotation)) * chasespeed;
                  yspeed += Math.sin(_root.angleToRadians(_rotation)) * chasespeed;
               }
               if(distance > 100 * scale || _root.playpause == true || stopchasing > 0 || _root.playpause == true)
               {
                  chase = false;
                  frame = "swim";
               }
            }
            xspeed *= 0.7;
            yspeed *= 0.7;
         }
         else
         {
            if(chase == true && lastframewater == true)
            {
               xspeed *= 2;
               _rotation = 0;
               yspeed = -11;
               frame = "jump";
            }
            lastframewater = false;
            yspeed += _root.gravity;
         }
         mariowater = this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water);
         marioinvincible = _root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3 || Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T";
         if(this.body.hitTest(_root.Course.Char.charClip.body) && marioinvincible)
         {
            xspeed += _root.Course.Char.xspeed;
            yspeed += _root.Course.Char.yspeed - 5;
            frame = "spin";
            spinspeed = Math.abs(_root.Course.Char.xspeed);
            spincount = 0;
         }
         if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            yspeed -= 1.4 / scale;
            xspeed += (Math.random() * 4 - 2 - xspeed / 1.5) / scale;
            frame = "spin";
            spinspeed = Math.abs(_root.Course.Char.xspeed);
            spincount = 0;
         }
         else
         {
            if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += Math.max((5 - xspeed / 1.5) / scale,0);
               frame = "spin";
               spinspeed = Math.abs(_root.Course.Char.xspeed);
               spincount = 0;
            }
            if(_root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               yspeed -= 1.4 / scale;
               xspeed += Math.min((-5 - xspeed / 1.5) / scale,0);
               frame = "spin";
               spinspeed = Math.abs(_root.Course.Char.xspeed);
               spincount = 0;
            }
            if(mariowater)
            {
               frame = "spin";
               spinspeed = Math.abs(_root.Course.Char.xspeed);
               spincount = 0;
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
            if(this.body.hitTest(_root.Course.Char.charClip.body) && !marioinvincible && frame !== "spin" && frame !== "gone")
            {
               stopchasing = 32;
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(1,90,-5,7,7,"");
               }
               else
               {
                  _root.MarioHit(1,90,-5,-7,7,"");
               }
            }
         }
      }
      else if(frame == "spin")
      {
         xspeed *= 0.85;
         yspeed *= 0.85;
         spincount++;
         clip.frame._rotation += spinspeed * 6;
         if(spincount > 16)
         {
            frame = "gone";
         }
      }
      else if((Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600))
      {
         _Y = saveY;
         _X = saveX;
         restartvars();
      }
      if(HitTest("Platforms",_X,_Y + tall * scale))
      {
         while(HitTest("Platforms",_X,_Y + tall * scale - 1))
         {
            _Y--;
         }
         if(lastframewater == false)
         {
            yspeed = -4;
            xspeed *= -0.5;
            xspeed += Math.random() * 4 - 2;
            if(frame !== "gone" && frame !== "spin")
            {
               frame = "swim";
            }
            if(xspeed < 0 && _xscale < 0)
            {
               _xscale = _xscale * -1;
            }
            if(xspeed > 0 && _xscale > 0)
            {
               _xscale = _xscale * -1;
            }
         }
         else
         {
            yspeed = (- yspeed) * 0.7;
         }
      }
      if(HitTest("Platforms",_X,_Y - tall * scale))
      {
         while(HitTest("Platforms",_X,_Y - tall * scale))
         {
            _Y++;
         }
         yspeed = (- yspeed) * 0.7;
      }
      if(HitTest("Platforms",_X + wide * scale,_Y))
      {
         while(HitTest("Platforms",_X + wide * scale,_Y))
         {
            _X--;
         }
         xspeed = (- xspeed) * 0.7;
      }
      if(HitTest("Platforms",_X - wide * scale,_Y))
      {
         while(HitTest("Platforms",_X - wide * scale,_Y))
         {
            _X++;
         }
         xspeed = (- xspeed) * 0.7;
      }
      _X = _X + xspeed;
      _Y = _Y + yspeed;
      clip.gotoAndStop(frame);
   }
   else if(frame == "gone")
   {
      if((Math.abs(_root.currentCamX - (_X + _parent._x)) > 600 || Math.abs(_root.currentCamY - (_Y + _parent._y)) > 600) && (Math.abs(_root.currentCamX - (saveX + _parent._x)) > 600 || Math.abs(_root.currentCamY - (saveY + _parent._y)) > 600))
      {
         _Y = saveY;
         _X = saveX;
         restartvars();
      }
   }
};
