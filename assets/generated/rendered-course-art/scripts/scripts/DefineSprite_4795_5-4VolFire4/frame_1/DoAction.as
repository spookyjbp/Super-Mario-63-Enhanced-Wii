yspeed = - (random(13) + 9);
xspeed = (random(6) - 2.5) * 3;
rotspeed = (random(6) - 2.5) * 3;
onEnterFrame = function()
{
   _Y = _Y + yspeed;
   _X = _X + xspeed;
   _rotation = _rotation + rotspeed;
   yspeed += 0.5;
   yspeed *= 0.98;
   xspeed *= 0.99;
   if(end == true)
   {
      this.removeMovieClip();
      stop();
      yspeed = 0;
      xspeed = 0;
   }
};
