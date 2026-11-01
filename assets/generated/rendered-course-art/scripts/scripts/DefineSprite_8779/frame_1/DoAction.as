al = 50;
_alpha = 50;
onEnterFrame = function()
{
   if(_parent.greyness.aa == 0)
   {
      if(this.hitTest(_root._xmouse,_root._ymouse,false))
      {
         sc = 120;
         al = 100;
      }
      else
      {
         sc = 100;
         al = 50;
      }
      _xscale = _xscale + (sc - _xscale) / 2;
      _yscale = _xscale;
      _alpha = _alpha + (al - _alpha) / 2;
      if(_parent.chase == true)
      {
         gotoAndStop(4);
         _alpha = 100;
      }
      else
      {
         gotoAndStop(3);
      }
   }
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _xscale = 140;
      _yscale = 140;
      _parent.chase = !_parent.chase;
   }
};
if(_parent.chase == undefined)
{
   _parent.chase = false;
}
