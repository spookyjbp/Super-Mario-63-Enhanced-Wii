stop();
_rotation = random(360);
rotspeed = (random(10) - 5) / 2;
speed = 3 + Math.random() * 2;
_Y = - _root.Course.BackGFX._y - 500 - random(500);
_X = - _root.Course.BackGFX._x + Math.random() * 1000 - 500;
mariox = _root.Course.Char._x - _root.Course.BackGFX._x;
marioy = _root.Course.Char._y - _root.Course.BackGFX._y;
dy = _Y - marioy;
dx = _X - mariox;
rot = Math.atan2(dy,dx + Math.random() * 1000 - 500);
yspeed = (- Math.sin(rot)) * speed;
xspeed = (- Math.cos(rot)) * speed;
_alpha = 100;
bcount = 100 + random(1000);
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _rotation = _rotation + rotspeed;
      _X = _X + xspeed;
      _Y = _Y + yspeed;
      if(this.body.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char.attackFrame !== "SlingStar" && _root.Course.Char.attackFrame !== "JumpSpin" || _root.Course.Char.attack == false)
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
         Explode();
      }
      if(count++ > bcount)
      {
         Explode();
      }
   }
};
