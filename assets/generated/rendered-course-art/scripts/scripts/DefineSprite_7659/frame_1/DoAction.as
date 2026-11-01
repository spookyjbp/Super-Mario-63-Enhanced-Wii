gotoAndStop(_parent.txt);
hl._alpha = 0;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      hl._alpha += (100 - hl._alpha) / 3;
      Y = Math.max(Math.min(Math.round((this._ymouse - 7.5) / 15),array.length - 1),0);
      Y *= 15;
      hl._y = Y;
   }
   else
   {
      hl._alpha += (- hl._alpha) / 3;
   }
   if(_parent._parent.cl !== _parent.txt)
   {
      _parent.rev = true;
   }
};
onMouseDown = function()
{
   if(_parent._parent.cl == _parent.txt)
   {
      _parent._parent.cl = "";
   }
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && isNaN(Y) == false)
   {
      _root.M[arrayf[Y / 15]]();
      _parent.rev = true;
   }
};
