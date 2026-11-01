num = -5;
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
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   _rotation = _rotation + (- _rotation) / 2;
   if(hold == true && sc == 105)
   {
      if(count == 0 || count > 10 && count % 2 == 0)
      {
         _parent.rot += num;
         _rotation = num * 2;
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
