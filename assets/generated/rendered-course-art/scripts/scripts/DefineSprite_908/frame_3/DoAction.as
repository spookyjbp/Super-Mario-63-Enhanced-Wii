stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(attack == false)
      {
         if(totalwindowsopen == 2 && HP > 2)
         {
            _root.TextHint = "(Now <b>Ram</b> into King Boo using Turbo Fludd!)";
         }
         else if(_root.TextHint == "(Now <b>Ram</b> into King Boo using Turbo Fludd!)")
         {
            _root.TextHint = "";
         }
      }
      totalwindowsopen = 0;
      if(_root.Course.BackGFX.w1._currentframe >= 2 && _root.Course.BackGFX.w1._currentframe <= 11)
      {
         totalwindowsopen = 1;
         if(_root.Course.BackGFX.w2._currentframe >= 2 && _root.Course.BackGFX.w2._currentframe <= 11)
         {
            totalwindowsopen = 2;
         }
      }
      if(_root.Course.BackGFX.w2._currentframe >= 2 && _root.Course.BackGFX.w2._currentframe <= 11)
      {
         totalwindowsopen = 1;
         if(_root.Course.BackGFX.w1._currentframe >= 2 && _root.Course.BackGFX.w1._currentframe <= 11)
         {
            totalwindowsopen = 2;
         }
      }
      if(attack == false)
      {
         _alpha = totalwindowsopen * 45 + 10;
         if(this.clip.body.hitTest(_root.Course.Char.charClip.body))
         {
            if(totalwindowsopen >= 2 && Math.abs(_root.Course.Char.xspeed) > 7)
            {
               HP -= 1;
               _alpha = 100;
               attack = true;
               if(HP <= 0)
               {
                  attackFrame = "defeated";
               }
               else
               {
                  attackFrame = "hurt";
               }
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.MarioHit(0,0,-5,7,15,"none");
               }
               else
               {
                  _root.MarioHit(0,0,-5,-7,15,"none");
               }
            }
            else if(totalwindowsopen < 2)
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
         }
      }
      if(attack == false || attack == true && attackFrame == "turn")
      {
         xdistance = _root.Course.Char._x - (_X + _parent._x);
         ydistance = _root.Course.Char._y - (_Y + _parent._y);
         distance = Math.sqrt(Math.pow(xdistance,2) + Math.pow(ydistance,2));
         rotspeed = Math.abs(xspeed / 2) + 2;
         rotmovement += rotspeed / 30;
         yspeed += Math.sin(rotmovement) * 0.2;
         if(frame == "move")
         {
            yspeed += Math.min(Math.max(ydistance / 50,-0.3),0.3);
            xspeed += Math.min(Math.max(xdistance / 20,-1.5),1.5);
         }
         if(yspeed > 0.7)
         {
            movedir = "down";
         }
         else if(yspeed < -0.7)
         {
            movedir = "up";
         }
         else
         {
            movedir = "none";
         }
      }
      if(attack == false)
      {
         if(xspeed > 1 && _xscale > 0)
         {
            attack = true;
            attackFrame = "turn";
         }
         if(xspeed < -1 && _xscale < 0)
         {
            attack = true;
            attackFrame = "turn";
         }
      }
      if(attack == false)
      {
         clip.gotoAndStop(frame);
      }
      else
      {
         clip.gotoAndStop(attackFrame);
      }
      xspeed *= 0.8;
      yspeed *= 0.8;
      _X = _X + xspeed;
      _Y = _Y + yspeed;
   }
};
