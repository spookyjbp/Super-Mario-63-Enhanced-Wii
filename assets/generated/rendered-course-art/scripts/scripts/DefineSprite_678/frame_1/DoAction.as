onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      sc = 120;
   }
   else
   {
      sc = 100;
   }
   if(_parent.sm._currentframe == 3)
   {
      al = 100;
      gotoAndStop(2);
   }
   else
   {
      sc = 75;
      al = 50;
      gotoAndStop(3);
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   _alpha = _alpha + (al - _alpha) / 2;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _xscale = 140;
      _yscale = 140;
      _parent.sm.gotoAndStop("blank");
      _parent.sm.gotoAndStop(2);
   }
};
gotoAndStop(2);
