gotoAndStop(_parent.rotdirection);
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
   gotoAndStop(_parent.rotdirection);
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(hl == true)
   {
      if(_parent.rotdirection == "Right")
      {
         _parent.rotdirection = "Left";
      }
      else
      {
         _parent.rotdirection = "Right";
      }
   }
};
