stop();
attack = true;
frame = "atk1";
clip.gotoAndStop(frame);
clip.frame.fireball = [3,3,3];
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(hp > 0)
      {
         _root.Course.BackGFX.camCenter._x = (_X + (_root.Course.Char._x - _root.Course.BackGFX._x) * 2) / 3;
         _root.Course.BackGFX.camCenter._y = (_Y + (_root.Course.Char._y - _root.Course.BackGFX._y) * 1) / 2;
         distance = Math.sqrt(Math.pow(_X - (_root.Course.Char._x - _root.Course.BackGFX._x),2) + Math.pow(_Y - (_root.Course.Char._y - _root.Course.BackGFX._y),2));
         if(_root.CameraZoom < 100 - Math.max(distance / 4 - 100,0))
         {
            _root.CameraZoom = _root.CameraZoom + 1;
         }
         if(_root.CameraZoom > 95 - Math.max(distance / 4 - 100,0))
         {
            _root.CameraZoom = _root.CameraZoom - 1;
         }
         _root.Camcenter = _root.Course.BackGFX.camCenter;
      }
      if(frame == "Idle" || attack == false || attack == true && (frame == "move" || frame == "summon"))
      {
         _Y = _Y + Math.sin(sinwave);
         sinwave += 0.1;
         if(attack == false && frame !== "move")
         {
            _Y = _Y - (_Y - targety) / 30;
            _X = _X - (_X - targetx) / 30;
         }
      }
      else
      {
         sinwave = 0;
      }
      if(attack == false)
      {
         aiwait++;
         if(aiwait >= 64 - diffcounter * 2)
         {
            aicount++;
            if(aicount > 6)
            {
               aicount = 0;
            }
            aiwait = 0;
            attack = true;
            if(Math.abs(_X - (_root.Course.Char._x - _parent._x)) < 80 && aicount !== 2 && aicount !== 4 && aicount !== 5)
            {
               yspeed = 0;
               frame = "stomp";
            }
            else if(aicount !== 2 && aicount !== 4 && aicount !== 5)
            {
               diffcounter = (3 - hp) * 3;
               frame = "atk1";
               clip.gotoAndStop(frame);
               randomnum = random(7 + diffcounter) + diffcounter;
               randomnum = Math.min(randomnum,18);
               switch(randomnum)
               {
                  case 0:
                     clip.frame.fireball = [0,0,0];
                     break;
                  case 1:
                     clip.frame.fireball = [1,1];
                     break;
                  case 2:
                     clip.frame.fireball = [2,0];
                     break;
                  case 3:
                     clip.frame.fireball = [2,2];
                     break;
                  case 4:
                     clip.frame.fireball = [1,4];
                     break;
                  case 5:
                     clip.frame.fireball = [3,3,3];
                     break;
                  case 6:
                     clip.frame.fireball = [3,2,0];
                     break;
                  case 7:
                     clip.frame.fireball = [2,2,2];
                     break;
                  case 8:
                     clip.frame.fireball = [4,0,0,0,0,0,0,0,0,0];
                     break;
                  case 9:
                     clip.frame.fireball = [4,4,4];
                     break;
                  case 10:
                     clip.frame.fireball = [3,2,2,3];
                     break;
                  case 11:
                     clip.frame.fireball = [3,1,1,3];
                     break;
                  case 12:
                     clip.frame.fireball = [0,1,2,3,3];
                     break;
                  case 13:
                     clip.frame.fireball = [1,1,1,1];
                     break;
                  case 14:
                     clip.frame.fireball = [3,4,4,3];
                     break;
                  case 15:
                     clip.frame.fireball = [4,4,4,4,4];
                     break;
                  case 16:
                     clip.frame.fireball = [3,3,3,3,3,3,3];
                     break;
                  case 17:
                     clip.frame.fireball = [2,2,2,2,2,2,2,2,2,2];
                     break;
                  case 18:
                     clip.frame.fireball = [1,1,1,1,1,1];
               }
            }
            else if(aicount == 2)
            {
               if(diffcounter2 == true)
               {
                  frame = "atk2pre";
               }
               else
               {
                  diffcounter2 = true;
                  frame = "move";
                  if(_X > 0)
                  {
                     targetx = -200;
                  }
                  else
                  {
                     targetx = 200;
                  }
               }
            }
            else if(aicount == 4)
            {
               frame = "summon";
               if(_X > 0)
               {
                  targetx = -60;
               }
               else
               {
                  targetx = 60;
               }
            }
            else if(aicount == 5)
            {
               frame = "atk2pre";
            }
         }
      }
      if((frame == "move" || frame == "summon") && attack == true)
      {
         _Y = _Y - (_Y - targety) / 15;
         if(targetx < 0)
         {
            if(_xscale > 0)
            {
               if(_X + xspeed * 10 > targetx)
               {
                  xspeed -= 1;
                  xspeed *= 0.9;
               }
               else
               {
                  xspeed *= 0.9;
                  clip.frame.play();
               }
            }
            else
            {
               xspeed *= 0.9;
               clip.frame.play();
            }
         }
         else if(_xscale < 0)
         {
            if(_X + xspeed * 10 < targetx)
            {
               xspeed += 1;
               xspeed *= 0.9;
            }
            else
            {
               xspeed *= 0.9;
               clip.frame.play();
            }
         }
         else
         {
            xspeed *= 0.9;
            clip.frame.play();
         }
      }
      if(frame == "atk2" && attack == true)
      {
         _Y = _Y - (_Y - targety) / 20;
         if(targetx < 0)
         {
            if(_xscale > 0)
            {
               if(_X - 120 > targetx)
               {
                  xspeed -= 0.25;
                  xspeed *= 0.94;
               }
               else
               {
                  xspeed *= 0.98;
                  clip.frame.play();
               }
            }
            else
            {
               xspeed *= 0.9;
               clip.frame.play();
            }
         }
         else if(_xscale < 0)
         {
            if(_X + 120 < targetx)
            {
               xspeed += 0.25;
               xspeed *= 0.94;
            }
            else
            {
               xspeed *= 0.98;
               clip.frame.play();
            }
         }
         else
         {
            xspeed *= 0.9;
            clip.frame.play();
         }
      }
      if(frame == "stomp" && attack == true)
      {
         aiwait++;
         if(aiwait > 4)
         {
            yspeed += 1;
            if(_Y > -40)
            {
               yspeed *= -1;
            }
         }
         if(yspeed > 0)
         {
            xspeed = (- (_X - (_root.Course.Char._x - _root.Course.BackGFX._x))) / 15;
         }
         else
         {
            _X = _X - (_X - targetx) / 20;
            xspeed = 0;
         }
         if(aiwait > 24 && yspeed > 0)
         {
            xspeed = 0;
            yspeed = 0;
            targety = -200;
            aiwait = 0;
            attack = false;
         }
      }
      if(attack == false)
      {
         xspeed *= 0.9;
      }
      if(hp > 0)
      {
         if(this.blade.hitTest(_root.Course.Char.charClip.body))
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
         if(this.shell.hitTest(_root.Course.Char.charClip.body))
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
         if(this.head.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.yspeed > 0 && _root.Course.Char.air == true && frame == "atk2" && attack == true)
         {
            hp--;
            if(hp > 0)
            {
               attack = true;
               frame = "hurt";
            }
            else
            {
               attack = true;
               frame = "hurt2";
            }
            diffcounter2 = false;
            if(_root.Course.Char.attackFrame == "Ground" && _root.Course.Char.attack == true)
            {
               _root.Course.Char.yspeed /= 2;
               _root.Course.Char.yspeed -= 12;
               _root.Course.Char.xspeed /= 2;
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.Course.Char.xspeed += 7;
               }
               else
               {
                  _root.Course.Char.xspeed -= 7;
               }
            }
            else
            {
               _root.Course.Char.yspeed /= 2;
               _root.Course.Char.yspeed -= 9;
               _root.Course.Char.xspeed /= 1.5;
               if(_root.Course.Char._x > _X + _parent._x)
               {
                  _root.Course.Char.xspeed += 8;
               }
               else
               {
                  _root.Course.Char.xspeed -= 8;
               }
            }
         }
      }
      _X = _X + xspeed;
      _Y = _Y + yspeed;
      if(attack == false)
      {
         frame = "Idle";
      }
      clip.gotoAndStop(frame);
   }
};
