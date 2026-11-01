num = Number(_name);
gotoAndStop(num + 2);
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      xs = 140;
   }
   else
   {
      xs = 100;
   }
   if(currentvoteisthis == true)
   {
      xs = 95;
      _alpha = _alpha + (50 - _alpha) / 2;
   }
   else
   {
      _alpha = _alpha + (100 - _alpha) / 2;
   }
   _xscale = _xscale + (xs - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(xs == 140)
   {
      _xscale = 200;
      _parent._parent.voteani.cli.play();
      _parent._parent.voteani.cli.num = num;
      _parent._parent.voteani.cli._x = num * 15;
   }
};
