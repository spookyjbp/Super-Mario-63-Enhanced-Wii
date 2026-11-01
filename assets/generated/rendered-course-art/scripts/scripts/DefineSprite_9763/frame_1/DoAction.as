onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      xs = 120;
   }
   else
   {
      xs = 100;
   }
   _xscale = _xscale + (xs - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(xs == 120)
   {
      getURL("http://runouw.com/levels/viewlevel.php?id=" + _root.LDSelectedLevel,"_blank");
   }
};
