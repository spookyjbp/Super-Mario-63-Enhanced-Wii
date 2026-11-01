if(_rotation == 0)
{
   num = 1;
}
else
{
   _alpha = 0;
   al = 0;
   num = -1;
}
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
   if(_parent.helpfra + num > 0 && _parent.helpfra + num < 7)
   {
      al = 100;
   }
   else
   {
      sc = 50;
      al = 0;
   }
   _alpha = _alpha + (al - _alpha) / 2;
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _parent.helpfra += num;
   }
};
gotoAndStop(frame + 1);
