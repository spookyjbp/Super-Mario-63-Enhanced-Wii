Re = function()
{
   _xscale = random(30) + 30;
   _yscale = random(10) + 10;
   speed = (random(10) + 10) / 2;
};
Re();
_X = random(800);
onEnterFrame = function()
{
   _X = _X - speed;
   if(_X < -100)
   {
      _X = 500 + random(400);
      Re();
   }
};
