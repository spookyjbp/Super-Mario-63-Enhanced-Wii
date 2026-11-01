frame = Number(_name.charAt(1));
if(_parent.helpfra == undefined)
{
   _parent.helpfra = 1;
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
   if(_parent.helpfra == frame)
   {
      sc = 80;
      b.gotoAndStop(2);
   }
   else
   {
      b.gotoAndStop(1);
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _parent.helpfra = frame;
   }
};
b.stop();
gotoAndStop(frame + 1);
