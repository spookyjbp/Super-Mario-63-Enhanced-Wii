onEnterFrame = function()
{
   if(_root.Fluddpow == "" || this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      sc = 115;
   }
   else
   {
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   if(Math.abs(sc - _xscale) < 1)
   {
      _xscale = sc;
   }
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && _root.CanSwitchFludd !== false)
   {
      _root.Fluddpow = "";
   }
};
