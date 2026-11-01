stop();
gotoAndStop(Number(_name.charAt(1)) + 1);
if(_parent.Tab == this._currentframe - 1)
{
   hl._alpha = 100;
}
else
{
   hl._alpha = 0;
}
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _root.MWindow1 == true)
   {
      _xscale = _xscale + (105 - _xscale) / 3;
   }
   else
   {
      _xscale = _xscale + (100 - _xscale) / 3;
   }
   if(_parent.moved.Tab == this._currentframe - 1)
   {
      hl._alpha = 100;
   }
   else
   {
      hl._alpha = 0;
   }
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _root.MWindow1 == true)
   {
      _parent.moved.Tab = this._currentframe - 1;
      _parent.moved.Reload();
      _parent.bar.drag._y = -40;
      _parent.bar.drag.Y = -80;
   }
};
