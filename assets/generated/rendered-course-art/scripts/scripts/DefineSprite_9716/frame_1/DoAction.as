onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _parent.Selected !== "" && _parent.Selected !== undefined)
   {
      xs = 120;
   }
   else
   {
      xs = 100;
   }
   if(_parent.Selected == "" || _parent.Selected == undefined)
   {
      _alpha = _alpha + (- _alpha) / 2;
   }
   else
   {
      _alpha = _alpha + (100 - _alpha) / 2;
   }
   _xscale = _xscale + (xs - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(xs == 120)
   {
      _root.LDSelectedLevel = _parent.box["Row" + _parent.Selected].num;
      trace(_parent.box["Row" + _parent.Selected].num);
      _parent.N("Lev");
   }
};
_alpha = 0;
