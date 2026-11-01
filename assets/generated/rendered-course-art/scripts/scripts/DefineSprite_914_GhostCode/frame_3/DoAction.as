stop();
yspeed = 0;
xspeed = 0;
rotmovement = 0;
rotspeed = 2;
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      xdistance = _root.Course.Char._x - (_X + _parent._x);
      ydistance = _root.Course.Char._y - (_Y + _parent._y);
      distance = Math.sqrt(Math.pow(xdistance,2) + Math.pow(ydistance,2));
      if(frame !== "Die")
      {
         if(Math.abs(distance) < 500 && Math.abs(ydistance) < 300)
         {
            if(Math.abs(ydistance) < Math.abs(xdistance) * 2)
            {
               if(xdistance < -10 && _root.Course.Char._xscale > 0 && _xscale < 0)
               {
                  frame = "Chase";
                  xspeed -= 0.5;
                  yspeed += Math.min(Math.max(ydistance / 30,-0.3),0.3);
                  _xscale = -100;
               }
               else if(xdistance < -10 && _root.Course.Char._xscale < 0 && _xscale < 0)
               {
                  frame = "Hide";
               }
               else if(_xscale < 0)
               {
                  frame = "Idle";
               }
               if(xdistance > 10 && _root.Course.Char._xscale < 0 && _xscale > 0)
               {
                  frame = "Chase";
                  xspeed += 0.5;
                  yspeed += Math.min(Math.max(ydistance / 30,-0.3),0.3);
                  _xscale = 100;
               }
               else if(xdistance > 10 && _root.Course.Char._xscale > 0 && _xscale > 0)
               {
                  frame = "Hide";
               }
               else if(_xscale > 0)
               {
                  frame = "Idle";
               }
            }
            else
            {
               frame = "Idle";
            }
         }
         else
         {
            frame = "Idle";
         }
      }
      if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == true && _root.Course.Char.yspeed > 0 && frame !== "Die" && frame !== "Hide")
      {
         frame = "Die";
         yspeed += Math.max(_root.Course.Char.yspeed,0) + 5;
         xspeed += _root.Course.Char.xspeed / 2;
         _root.Course.Char.yspeed /= 2;
         _root.Course.Char.yspeed -= 13;
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("MarioImpact");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
      }
      if(this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3) && frame !== "Die" && frame !== "Hide")
      {
         frame = "Die";
         yspeed -= 7;
         xspeed += _root.Course.Char.xspeed / 2;
      }
      if(this.body.hitTest(_root.Course.Char.charClip.body) && (Math.abs(_root.Course.Char.xspeed) >= 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T") && frame !== "Die" && frame !== "Hide")
      {
         frame = "Die";
         yspeed -= 7;
         xspeed += _root.Course.Char.xspeed / 2;
      }
      if(this.body.hitTest(_root.Course.Char.charClip.water) || this.body.hitTest(_root.Course.Char.charClip.frame.water) || this.head.hitTest(_root.Course.Char.charClip.water) || this.head.hitTest(_root.Course.Char.charClip.frame.water) && frame !== "Die" && frame !== "Hide")
      {
         frame = "Die";
         yspeed -= 9;
         xspeed += Math.random() * 8 - 4;
      }
      if(this.body.hitTest(_root.Course.Char.charClip.body) && (_root.Course.Char.attack == false || !(_root.Course.Char.attackFrame == "Dive" && _root.Course.Char.attack == true && Math.abs(_root.Course.Char.xspeed) > 0.3)) && !(Math.abs(_root.Course.Char.xspeed) > 7 && _root.KeySPACE() && _root.Power >= 100 && _root.WaterAmount > 0 && _root.Fluddpow == "T") && frame !== "Die" && frame !== "Hide")
      {
         if(_root.Course.Char.blink <= 0)
         {
            sound = true;
         }
         else
         {
            sound = false;
         }
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(1,90,-5,7,7,"");
         }
         else
         {
            _root.MarioHit(1,90,-5,-7,7,"");
         }
         if(_root.Course.Char.blink > 0 && sound == true)
         {
            _root.Mariosound = new Sound(this);
            _root.Mariosound.attachSound("Boo");
            _root.Mariosound.start(0,1);
            _root.Mariosound.setVolume(_root.MarioVolume);
         }
      }
      if(frame == "Idle")
      {
         if(_xscale > 0)
         {
            xspeed += 0.3;
         }
         else
         {
            xspeed -= 0.3;
         }
         count += random(3) + 1;
         if(count > 300)
         {
            count = 0;
            _xscale = _xscale * -1;
         }
      }
      if(frame == "Hide")
      {
         _alpha = _alpha - 5;
         _alpha = Math.max(_alpha,30);
      }
      else
      {
         _alpha = _alpha + 5;
         _alpha = Math.min(_alpha,100);
      }
      if(frame == "Die")
      {
         _alpha = _alpha - 10;
         clip._rotation += random(10) + 15;
         if(_alpha <= 30)
         {
            _alpha = Math.max(_alpha,0);
            frame = "Idle";
         }
      }
      if(frame !== "Die")
      {
         clip._rotation = _root.groundFriction(clip._rotation,5,1.2);
         rotspeed = Math.abs(xspeed) + 1;
         rotmovement += rotspeed / 30;
         xspeed *= 0.7;
         yspeed *= 0.7;
         clip._y = Math.sin(rotmovement) * 10;
      }
      else
      {
         xspeed *= 0.9;
         yspeed *= 0.7;
      }
      _yscale = scale * 100;
      if(_xscale > 0)
      {
         _xscale = scale * 100;
      }
      if(_xscale < 0)
      {
         _xscale = (- scale) * 100;
      }
      clip.gotoAndStop(frame);
      _X = _X + xspeed;
      _Y = _Y + yspeed;
   }
};
