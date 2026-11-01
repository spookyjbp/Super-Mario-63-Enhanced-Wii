if(loady == undefined)
{
   gotoAndPlay(random(100) + 1);
   Re = function()
   {
      loady = 4;
      _xscale = random(50) + 30;
      _yscale = random(25) + 15;
      speed = (random(5) + 5) / 2;
   };
   _X = random(800);
   Re();
   onEnterFrame = function()
   {
      _X = _X - speed;
      if(_X < -100)
      {
         _X = 500 + random(400);
         Re();
      }
   };
}
