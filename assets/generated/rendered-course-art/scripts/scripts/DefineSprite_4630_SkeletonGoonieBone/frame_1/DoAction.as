xspeed = Math.random() * 4 - 2;
yspeed = Math.random() * 3 - 3.5;
rotspeed = (random(2) - 0.5) * (Math.abs(xspeed) + Math.abs(yspeed)) * 10;
count = random(9);
_X = _X + (random(100) - 50) / 10;
_Y = _Y + (random(100) - 50) / 10;
_rotation = random(360);
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _X = _X + xspeed;
      _Y = _Y + yspeed;
      _rotation = _rotation + rotspeed;
      yspeed += 0.15;
      count++;
      if(count > 18)
      {
         _alpha = _alpha - 20;
         if(_alpha < 10)
         {
            this.swapDepths(0);
            this.removeMovieClip();
         }
      }
   }
   else
   {
      stop();
   }
};
