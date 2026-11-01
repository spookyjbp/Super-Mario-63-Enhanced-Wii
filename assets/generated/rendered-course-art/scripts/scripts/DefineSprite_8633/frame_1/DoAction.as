sin = 12;
sin2 = 45;
rotplus = 0;
Y = _Y;
onEnterFrame = function()
{
   if(_parent.greyness._alpha < 50)
   {
      sin += 0.041;
      sin2 += 0.023;
      _Y = 2 * Math.sin(sin2) + Y;
      this._rotation = rotplus + 2 * Math.sin(sin);
      if(this.hitTest(_root._xmouse,_root._ymouse,true))
      {
         rotplus += (5 - rotplus) / 3;
         _xscale = _xscale + (120 - _xscale) / 3;
         txt._xscale += (120 - txt._xscale) / 3;
      }
      else
      {
         rotplus += (- rotplus) / 3;
         _xscale = _xscale + (100 - _xscale) / 3;
         txt._xscale += (100 - txt._xscale) / 3;
      }
      _yscale = _xscale;
      txt._yscale = txt._xscale;
   }
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _parent.greyness._alpha < 20)
   {
      System.showSettings(1);
      _xscale = 150;
      _yscale = 150;
   }
};
