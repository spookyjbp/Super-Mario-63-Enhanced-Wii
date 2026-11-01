sin = 0;
onEnterFrame = function()
{
   sin += 0.02;
   _xscale = 120 + Math.sin(sin) * 10;
   _yscale = _xscale;
};
