front = false;
onEnterFrame = function()
{
   if(sfold !== _root.sideframe)
   {
      front = false;
   }
   if(front == false && _X > 452)
   {
      if(_root.sideframe !== undefined)
      {
         front = true;
      }
   }
   sfold = _root.sideframe;
   if(front == true)
   {
      this._x += (350 - _X) / 6;
      gotoAndStop(_root.sideframe);
   }
   else
   {
      this._x += (456 - _X) / 6;
      if(this._x > 450)
      {
         _root.PlaceMode = "";
         gotoAndStop(1);
      }
   }
   if((this.hitTest(_root._xmouse,_root._ymouse,true) || bar.held == true) && _root.MenuAccess !== true)
   {
      _root.MenuAccess = true;
      ma = true;
   }
   else if(ma == true && !(this.hitTest(_root._xmouse,_root._ymouse,true) || bar.held == true))
   {
      ma = false;
      _root.MenuAccess = false;
   }
};
stop();
