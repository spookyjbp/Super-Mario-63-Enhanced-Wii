onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && _parent.clicked !== true)
   {
      sc = 120;
   }
   else
   {
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 120 && _parent.clicked !== true)
   {
      _xscale = 140;
      _yscale = 140;
      _parent.clicked = true;
      _parent.Trans("starmap");
   }
};
