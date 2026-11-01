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
   if(_parent._parent.touchngo == false)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(3);
   }
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(hl == true)
   {
      if(_parent._parent.touchngo == false)
      {
         _parent._parent.touchngo = true;
      }
      else
      {
         _parent._parent.touchngo = false;
      }
   }
};
