al = 100;
timer = 64;
onEnterFrame = function()
{
   timer--;
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      al = 100;
      sc = 120;
      timer = 0;
   }
   else if(timer <= 0)
   {
      al = 10;
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   _alpha = _alpha + (al - _alpha) / 5;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _parent.Skip();
      _alpha = 100;
      sc = 100;
      timer = 64;
      _xscale = 140;
      _yscale = 140;
   }
};
