onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
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
      if(_root.Order == "hardest")
      {
         _root.Order = "easiest";
      }
      else
      {
         _root.Order = "hardest";
      }
      _parent.Refresh();
   }
};
stop();
