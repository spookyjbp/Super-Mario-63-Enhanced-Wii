onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      sc = 120;
   }
   else
   {
      sc = 100;
   }
   if(count++ < 5)
   {
      if(Boolean(Number(_parent.xs)) == true)
      {
         fr = 2;
         gotoAndStop(2);
      }
      else
      {
         fr = 11;
         gotoAndStop(11);
      }
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   if(_currentframe < fr)
   {
      nextFrame();
   }
   if(_currentframe > fr)
   {
      prevFrame();
   }
};
onMouseDown = function()
{
   if(sc == 120)
   {
      if(fr == 2)
      {
         fr = 11;
         _parent.xs = 0;
      }
      else
      {
         fr = 2;
         _parent.xs = 1;
      }
   }
};
count = 0;
if(Boolean(Number(_parent.xs)) == true)
{
   fr = 2;
   gotoAndStop(2);
}
else
{
   fr = 11;
   gotoAndStop(11);
}
