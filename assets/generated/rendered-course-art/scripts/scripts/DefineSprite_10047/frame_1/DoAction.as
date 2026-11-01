I = function(t)
{
   tellTarget(t)
   {
      xspeed = Math.random() * 8 - 4;
      yspeed = Math.random() * 4 - 8;
      rotspeed = random(16) - 8;
      count = 0;
   }
};
OEF = function(t)
{
   tellTarget(t)
   {
      yspeed += 0.2;
      _Y = _Y + yspeed;
      _X = _X + xspeed;
      _rotation = _rotation + rotspeed;
      if(_Y + _height / 2 > 86)
      {
         yspeed *= -0.8;
         _Y--;
         rotspeed = - rotspeed;
      }
      if(_X + _width / 2 > 177)
      {
         xspeed *= -0.9;
         _X--;
         rotspeed = - rotspeed;
      }
      if(_X - _width / 2 < -210)
      {
         xspeed *= -0.9;
         _X++;
         rotspeed = - rotspeed;
      }
      count++;
      if(count > 50 && Math.abs(yspeed) < 3 || count > 200)
      {
         _alpha = _alpha - 2;
         if(_alpha <= 0)
         {
            t.swapDepths(0);
            t.removeMovieClip();
         }
      }
   }
};
