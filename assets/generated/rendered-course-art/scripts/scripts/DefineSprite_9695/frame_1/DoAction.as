onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _root.Page > 0)
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
      _root.Page = _root.Page - 1;
      _parent.Refresh();
   }
};
stop();
