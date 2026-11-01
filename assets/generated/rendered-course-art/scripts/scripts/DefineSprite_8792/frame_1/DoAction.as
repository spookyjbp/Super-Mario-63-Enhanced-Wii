gotoAndStop(_parent.xdirection);
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
   gotoAndStop(_parent.xdirection);
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(hl == true)
   {
      if(_parent.xdirection == "Right")
      {
         _parent.xdirection = "Left";
      }
      else
      {
         _parent.xdirection = "Right";
      }
   }
};
