onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      hl = true;
      sc = 120;
   }
   else
   {
      hl = false;
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(hl == true)
   {
      _parent.CancelF();
   }
};
