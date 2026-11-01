stop();
if(_name !== "currenttab")
{
   if(_name.charAt(2) !== undefined)
   {
      gotoAndStop(Number(_name.charAt(1) + _name.charAt(2)) + 1);
   }
   else
   {
      gotoAndStop(Number(_name.charAt(1)) + 1);
   }
   array = [0,10,13,21,25,28,38,43];
   num = this._currentframe - 1;
   if(num >= 4 && num < 7)
   {
      if(_root.MaxStarCoins >= array[num - 3])
      {
         greuy._visible = false;
      }
      else
      {
         _alpha = 75;
         greuy._visible = true;
      }
   }
   else if(num >= 7)
   {
      if(_root.MaxStarCoins >= array[num - 4])
      {
         greuy._visible = false;
      }
      else
      {
         _alpha = 75;
         greuy._visible = true;
      }
   }
   else
   {
      greuy._visible = false;
   }
}
else
{
   greuy._visible = false;
}
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && greuy._visible == false && _root.MWindow1 == true && (_name !== "currenttab" || _parent._currentframe < 3))
   {
      _xscale = _xscale + (115 - _xscale) / 3;
      if(_name == "currenttab")
      {
         _parent._parent.tabchange = true;
      }
   }
   else
   {
      if(_name == "currenttab")
      {
         _parent._parent.tabchange = false;
      }
      _xscale = _xscale + (100 - _xscale) / 3;
   }
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && greuy._visible == false && _root.MWindow1 == true && _parent.selecting == true && _name !== "currenttab")
   {
      _parent.Tab = this._currentframe - 1;
      _parent._parent.bar.drag._y = -40;
      _parent._parent.bar.drag.Y = -80;
   }
};
