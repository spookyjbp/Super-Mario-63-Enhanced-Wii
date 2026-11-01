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
      _Y = 2 * Math.sin(sin2) + Y;
      this._rotation = - rotplus + 2 * Math.sin(sin);
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
      _parent.LoadCourse("code");
      _xscale = 200;
      _yscale = 200;
   }
};
