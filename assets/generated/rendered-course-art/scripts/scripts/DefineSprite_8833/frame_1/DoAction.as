s = _name.charAt(1);
xc._alpha = 0;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      hl = true;
      sc = 120;
   }
   else
   {
      hl = false;
      sc = 100;
   }
   if(_parent.sizevar == s)
   {
      xc._alpha += (100 - xc._alpha) / 2;
   }
   else
   {
      xc._alpha += (- xc._alpha) / 2;
   }
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
   txt._xscale += (sc - txt._xscale) / 3;
   txt._yscale = txt._xscale;
};
onMouseDown = function()
{
   if(hl == true)
   {
      _parent.sizevar = s;
   }
};
gotoAndStop(s);
