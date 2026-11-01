num = 1;
if(_rotation == 180)
{
   num = -1;
}
al = 100;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      sc = 105;
      gotoAndStop(3);
   }
   else
   {
      sc = 100;
      gotoAndStop(2);
   }
   if(_rotation == 180)
   {
      if(_parent._parent.Songnum > 1)
      {
         al = 100;
      }
      else
      {
         al = 0;
         sc = 100;
      }
   }
   else if(_rotation == 0)
   {
      if(_parent._parent.Songnum < 26)
      {
         al = 100;
      }
      else
      {
         al = 0;
         sc = 100;
      }
   }
   _alpha = _alpha + (al - _alpha) / 2;
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 105)
   {
      _parent._parent.Songnum += num;
      _xscale = 130;
      _yscale = 130;
   }
};
