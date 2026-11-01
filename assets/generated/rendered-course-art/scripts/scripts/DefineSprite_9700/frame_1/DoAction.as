onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _parent._parent.TotalPages > _root.Page + 1)
   {
      xs = 120;
      gotoAndStop(3);
   }
   else
   {
      xs = 100;
      gotoAndStop(2);
   }
   _xscale = _xscale + (xs - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(xs == 120)
   {
      _root.Page += 10;
      _root.Page = Math.min(_parent._parent.TotalPages,_root.Page);
      _parent.Refresh();
   }
};
stop();
