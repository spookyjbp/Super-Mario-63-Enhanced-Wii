_alpha = 0;
aa = 0;
onEnterFrame = function()
{
   if(_parent.greyness._alpha < 50)
   {
      if(this.hitTest(_root._xmouse,_root._ymouse,false))
      {
         aa = 50;
      }
      else
      {
         aa = 0;
      }
      _alpha = _alpha + (aa - _alpha) / 3;
   }
};
onMouseDown = function()
{
   if(_parent.greyness._alpha < 50)
   {
      if(this.hitTest(_root._xmouse,_root._ymouse,false))
      {
         aa = 100;
         _alpha = 100;
         _parent.gotoAndStop(fr);
      }
   }
};
