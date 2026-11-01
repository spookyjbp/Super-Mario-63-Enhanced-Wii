if(loaded == undefined)
{
   wide = 55;
   tall = 70;
   xspeed = -5;
   yspeed = 0;
   air = false;
   action = undefined;
   braincount = 0;
   num = 0;
   frame = 1;
   lastaction = "none";
   superjumptimer = 50;
   inv = true;
   hp = 1;
   SompedFunction = function()
   {
      if(inv == false)
      {
         if(hp < 3)
         {
            action = "stomped";
            frame = 4;
         }
         else
         {
            gotoAndStop(5);
            onEnterFrame = function()
            {
               gotoAndStop(5);
               if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
               {
                  while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
                  {
                     _Y--;
                  }
               }
               else
               {
                  _Y = _Y + 3;
               }
            };
         }
      }
   };
   onEnterFrame = function()
   {
      if(hp == 1)
      {
         _root.TextHint = "Hint: Press Z in the air to ground pound!";
      }
      if(_root.PauseGame == false)
      {
         marioX = _root.Course.Char._x - _root.Course.BackGFX._x - _X;
         marioY = _root.Course.Char._y - _root.Course.BackGFX._y - _Y;
         if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.attack == true && _root.Course.Char.attackFrame == "Ground" && _root.Course.Char.yspeed > 0)
         {
            SompedFunction();
         }
         else if(this.head.hitTest(_root.Course.Char.charClip.feet) && (_root.Course.Char.attackFrame !== "Ground" || attack == false))
         {
            _root.Course.Char.yspeed /= 2;
            _root.Course.Char.yspeed -= 9;
            _root.Course.Char.xspeed -= _xscale / 50;
         }
         else if(this.body.hitTest(_root.Course.Char.charClip.body) && action !== "stomped")
         {
            if(marioX > 0)
            {
               _root.MarioHit(1,90,-3,5,7,"");
            }
            else
            {
               _root.MarioHit(1,90,-3,-5,7,"");
            }
         }
         _root.Course.Enemyhurt.attachMovie("EnemyHurtBox","Box" + _root.Course.Enemyhurt.getNextHighestDepth(),_root.Course.Enemyhurt.getNextHighestDepth(),{_x:_X,_y:_Y - 35,_yscale:40,_xscale:30,time:2});
         if(action == undefined && air == false)
         {
            braincount = 0;
            num += random(2) + 1;
            if(num > 10)
            {
               num = random(3) + 1;
            }
            superjumptimer += random(2) + hp;
            if(superjumptimer > 64)
            {
               superjumptimer = 0;
               num = 10;
            }
            if(num < 9)
            {
               if(marioX > 0)
               {
                  if(_xscale > 0)
                  {
                     num = 9;
                     inv = false;
                  }
                  else
                  {
                     inv = false;
                     action = "Right";
                  }
               }
               if(marioX < 0)
               {
                  if(_xscale < 0)
                  {
                     num = 9;
                     inv = false;
                  }
                  else
                  {
                     inv = false;
                     action = "Left";
                  }
               }
            }
            if(num == 9)
            {
               frame = 3;
               gotoAndStop(1);
               action = "jump";
               air = true;
               num++;
            }
            else if(num == 10)
            {
               gotoAndStop(1);
               action = "Super Jump";
               air = true;
               count = 0;
               county = 0;
               num++;
            }
         }
         if(action == "Right")
         {
            frame = 1;
            xspeed += (4 + hp - xspeed) / 7;
            braincount++;
         }
         else if(action == "Left")
         {
            frame = 1;
            xspeed += (- (4 + hp) - xspeed) / 7;
            braincount++;
         }
         if(braincount >= 30)
         {
            braincount = 0;
            lastaction = action;
            action = undefined;
         }
         if(action == "Super Jump")
         {
            frame = 2;
            if(_Y < -450 && count <= 50)
            {
               _Y = -450;
               yspeed = 0;
            }
            count++;
            if(count > 85)
            {
               action = "Fall";
               yspeed = Math.max(yspeed,10);
            }
            else if(count > 45)
            {
               xspeed = Math.min(Math.max(marioX / 5,-4),4);
               if(marioX > 3)
               {
                  xspeed += 1.5;
               }
               if(marioX < -3)
               {
                  xspeed -= 1.5;
               }
            }
         }
         else if(action == "Fell")
         {
            county++;
            if(county > 20)
            {
               county = 0;
               lastaction = action;
               action = undefined;
            }
         }
         if(action == "Super Jump" && _Y <= -300)
         {
            _root.Course.FrontGFX.Shadow._alpha += 20;
            _root.Course.FrontGFX.Shadow._alpha = Math.min(_root.Course.FrontGFX.Shadow._alpha,100);
            _root.Course.FrontGFX.Shadow._x = _X;
            _root.Course.FrontGFX.Shadow._y = 0;
         }
         else
         {
            _root.Course.FrontGFX.Shadow._alpha -= 10;
            _root.Course.FrontGFX.Shadow._alpha = Math.max(_root.Course.FrontGFX.Shadow._alpha,0);
            _root.Course.FrontGFX.Shadow._x = _X;
            _root.Course.FrontGFX.Shadow._y = 0;
         }
         if(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,_Y * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            if(air == true && yspeed > 0 && action !== "Fall" && action !== "stomped")
            {
               lastaction = action;
               action = undefined;
            }
            else if(action == "Fall")
            {
               action = "Fell";
            }
            yspeed = Math.min(yspeed,0);
            air = false;
         }
         else
         {
            air = true;
         }
         if(air == true && action !== "Super Jump")
         {
            yspeed++;
         }
         if(yspeed > -3)
         {
            while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y + 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true) && !_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
            {
               _Y++;
            }
         }
         while(_root.Course.Platforms.hitTest(_X * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - 3) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _Y--;
         }
         while(_root.Course.Platforms.hitTest((_X - wide / 2) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X++;
            xspeed = Math.max(xspeed,(- xspeed) * 0.7);
         }
         while(_root.Course.Platforms.hitTest((_X + wide / 2) * _root.coursescale / 100 + _parent._x * _root.coursescale / 100 + _root.Course._x,(_Y - tall / 2) * _root.coursescale / 100 + _parent._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _X--;
            xspeed = Math.min(xspeed,(- xspeed) * 0.7);
         }
         yspeed = Math.min(yspeed,15);
         _X = _X + xspeed;
         _Y = _Y + yspeed;
         gotoAndStop(frame);
         if(frame == 1)
         {
            if(Math.abs(xspeed) > 1)
            {
               clip.play();
            }
         }
         xspeed *= 0.9;
         Stomped = false;
      }
   };
   loaded = true;
}
