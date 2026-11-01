stop();
frame = 1;
attack = false;
attackFrame = "";
air = false;
xspeed = 0;
yspeed = 0;
idlecount = 0;
idlecount = 0;
tall = 50;
wide = 35;
throwcount = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
      {
         _Y++;
      }
      while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _Y--;
      }
      if(attack == true)
      {
         idlecount = 0;
      }
      idlecount++;
      idlecounts = idlecount / _root.framerate;
      if(idlecounts > 0.5 && attack == false && air == false)
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
      if(idlecounts > 1)
      {
         randomnum = random(3) + 1;
         if(randomnum == 1)
         {
            if(random(2) == 1)
            {
               randomnum = 3;
            }
            else
            {
               randomnum = 2;
            }
         }
         if(throwcount > random(4) + 2)
         {
            randomnum = 1;
         }
         if(attack == false && air == false && randomnum == 1)
         {
            if(_xscale > 0 && _X + _parent._x - _root.Course.Char._x > 15)
            {
               attack = true;
               attackFrame = "throw";
            }
            else if(_xscale < 0 && _X + _parent._x - _root.Course.Char._x < -15)
            {
               attack = true;
               attackFrame = "throw";
            }
            throwcount = 0;
         }
         else if(attack == false && air == false && randomnum == 2)
         {
            if(_xscale > 0 && _X + _parent._x - _root.Course.Char._x > 20)
            {
               frame = "walk";
            }
            else if(_xscale < 0 && _X + _parent._x - _root.Course.Char._x < -20)
            {
               frame = "walk";
            }
            idlecount = 0;
            idlecounts = 0;
            throwcount++;
         }
         else if(attack == false && air == false && randomnum == 3 && frame !== "walk")
         {
            attack = true;
            attackFrame = "jump";
            idlecount = 0;
            idlecounts = 0;
            throwcount++;
         }
      }
      if(idlecounts > 0.5 && _xscale > 0 && _X + _parent._x - _root.Course.Char._x < 30 && _X + _parent._x - _root.Course.Char._x > 0)
      {
         attack = true;
         attackFrame = "grab";
         frame = "idle";
      }
      else if(idlecounts > 1 && _xscale < 0 && _X + _parent._x - _root.Course.Char._x > -30 && _X + _parent._x - _root.Course.Char._x < 0)
      {
         attack = true;
         attackFrame = "grab";
         frame = "idle";
      }
      if(frame == "walk" && attack == false)
      {
         if(_xscale > 0)
         {
            xspeed -= 0.7;
         }
         else if(_xscale < 0)
         {
            xspeed += 0.7;
         }
         if(!(_X + _parent._x) - _root.Course.Char._x > 20 && !(_X + _parent._x) - _root.Course.Char._x < -20 || attack == true)
         {
            frame = "idle";
         }
      }
      if(crown.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == false)
      {
         if(_root.Course.Char._x > _X + _parent._x)
         {
            _root.MarioHit(1,30,5,7,0,"");
         }
         else
         {
            _root.MarioHit(1,30,5,-7,0,"");
         }
      }
      if(_root.Course.Enemyhurt.hitTest((_X - wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest((_X + wide) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 1.5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest((_X - wide / 2) * _root.coursescale / 100 + _parent._x * _root
      .coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest((_X + wide / 2) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.Enemyhurt.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         if(attack == false || attackFrame !== "hurt")
         {
            attack = true;
            attackFrame = "hurt";
            if(_root.Course.Char.attack == true && _root.Course.Char.attackFrame == "grabbed")
            {
               _root.Course.Char.attack = false;
            }
         }
      }
      xspeed *= 0.7;
      if(attack == false)
      {
         clip.gotoAndStop(frame);
      }
      else
      {
         clip.gotoAndStop(attackFrame);
      }
      if(_root.Course.Platforms.hitTest((_X + xspeed - _xscale / 15) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest((_X + xspeed - _xscale / 15) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 5) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         _X = _X + xspeed;
      }
      else
      {
         frame = 1;
      }
      if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         yspeed = Math.min(yspeed,0);
         while((_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)) && !(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true)))
         {
            _Y++;
         }
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 1) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) || _root.Course.BPlatforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
         }
      }
      else
      {
         yspeed += 0.5;
      }
      _Y = _Y + yspeed;
      _root.Course.Platforms["KingBombombGround" + depth]._y = _Y;
      _root.Course.Platforms["KingBombombGround" + depth]._x = _X;
   }
};
