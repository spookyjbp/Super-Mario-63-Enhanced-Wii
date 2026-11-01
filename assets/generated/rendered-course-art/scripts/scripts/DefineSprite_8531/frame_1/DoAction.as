sc = 100;
onEnterFrame = function()
{
   this._x = _parent.widt;
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _parent.greyness._alpha < 20)
   {
      sc = 120;
   }
   else
   {
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 120 && _parent.greyness._alpha < 20)
   {
      _parent.frame = undefined;
      _parent.gotoAndStop("blank");
      _parent._y = 1000;
   }
};
