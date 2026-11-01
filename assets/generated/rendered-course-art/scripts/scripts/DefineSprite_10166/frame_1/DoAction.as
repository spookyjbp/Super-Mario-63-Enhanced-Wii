Re = function()
{
   gotoAndStop(random(9) + 2);
   _rotation = random(360);
   rotspeed = random(10) - 5;
   _X = random(500) - 250;
   _Y = -250 - random(100);
   xspeed = random(10) / 5 - 1;
   yspeed = 6 + random(10) / 5;
};
Re();
_Y = _Y - random(500);
onEnterFrame = function()
{
   _X = _X + xspeed;
   _Y = _Y + yspeed;
   _rotation = _rotation + rotspeed;
   if(_Y > 300)
   {
      Re();
   }
};
