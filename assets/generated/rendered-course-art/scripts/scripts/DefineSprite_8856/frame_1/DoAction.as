onEnterFrame = function()
{
   gotoAndStop(_parent.blocktype);
   _xscale = Math.min(_parent.sizevar / 2,80);
   _yscale = _xscale;
};
