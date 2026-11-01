Re = function()
{
   _xscale = random(40) + 80;
   _yscale = random(40) + 80;
   rot = random(360);
   rotspeed = random(30) / 2;
   speed = (random(5) + 5) / 4;
   xspeed = Math.sin(rot) * speed;
   yspeed = Math.abs(Math.cos(rot) * speed);
   if(count == undefined)
   {
      count = 0;
   }
   _alpha = 100;
};
Re();
count = random(150);
i = 0;
while(i < count)
{
   i++;
}
onEnterFrame = function()
{
   _rotation = _rotation + rotspeed;
   _X = _X + xspeed;
   _Y = _Y + yspeed;
   count++;
   if(count > 150)
   {
      _alpha = _alpha - 5;
   }
   if(_alpha <= 0)
   {
      this.removeMovieClip();
   }
};
