num = 4;
if(_rotation == 180)
{
   num = -4;
}
y2 = _Y;
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
      gotoAndStop(2);
      sc = 100;
   }
   if(_rotation == 0)
   {
      if(_parent.sm._y >= 0)
      {
         al = 0;
         sc = 100;
      }
      else
      {
         al = 100;
      }
   }
   else if(_rotation == 180)
   {
      if(_parent.sm._y <= - _parent.sm._height + 100)
      {
         al = 0;
         sc = 100;
      }
      else
      {
         al = 100;
      }
   }
   _alpha = _alpha + (al - _alpha) / 2;
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   _Y = _Y + (y2 - _Y) / 2;
   if(hold == true && sc == 105)
   {
      if(count == 0 || count > 6)
      {
         _parent.sm._y += num;
         _Y = _Y + num / 5;
         _xscale = 130;
         _yscale = 130;
      }
      count++;
   }
};
onMouseDown = function()
{
   if(sc == 105)
   {
      hold = true;
      count = 0;
   }
};
onMouseUp = function()
{
   hold = false;
};
