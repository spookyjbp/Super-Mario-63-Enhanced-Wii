sin = 0;
sin2 = random(90);
rotplus = 0;
Y = _Y;
onEnterFrame = function()
{
   if(_parent.greyness._alpha < 50)
   {
      sin += 0.05;
      sin2 += 0.03;
      _Y = 3 * Math.sin(sin2) + Y;
      this._rotation = - rotplus + 3 * Math.sin(sin);
      if(this.hitTest(_root._xmouse,_root._ymouse,true) || cl == true)
      {
         if(cl !== true)
         {
            rotplus += (10 - rotplus) / 3;
            _xscale = _xscale + (130 - _xscale) / 3;
            txt._xscale += (130 - txt._xscale) / 3;
         }
         else
         {
            rotplus += (100 - rotplus) / 8;
            _alpha = _alpha - 8;
            _xscale = _xscale + (200 - _xscale) / 4;
            txt._xscale += (200 - txt._xscale) / 4;
         }
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
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && rotplus >= 8.5 && _parent.greyness._alpha < 20)
   {
      _parent.FunctionContinue();
      cl = true;
   }
};
