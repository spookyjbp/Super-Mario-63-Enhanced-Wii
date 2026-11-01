stop();
tempparentwidt;
onEnterFrame = function()
{
   if(tempparentwidt !== _parent.widt)
   {
      this._width = _parent.widt - 10;
      tempparentwidt = _parent.widt;
   }
   if(_parent.greyness._alpha > 95)
   {
      drag = false;
   }
   if(drag == true)
   {
      _parent._x = Math.min(Math.max(_root._xmouse - lastx,1 - _parent._width + 20),429);
      _parent._y = Math.min(Math.max(_root._ymouse - lasty,3),280);
   }
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && _parent.greyness._alpha < 50)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      drag = true;
      lastx = _root._xmouse - _parent._x;
      lasty = _root._ymouse - _parent._y;
   }
};
onMouseUp = function()
{
   drag = false;
};
