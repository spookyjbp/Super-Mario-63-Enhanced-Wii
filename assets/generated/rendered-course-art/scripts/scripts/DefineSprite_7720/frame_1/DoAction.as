sc = 100;
onEnterFrame = function()
{
   this._x = _parent.widt;
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
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
   if(sc == 120)
   {
      _root.sideframe = undefined;
   }
};
