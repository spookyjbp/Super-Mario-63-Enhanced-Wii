widt = 250;
heigh = 210;
greyness.aa = 0;
greyness._alpha = 0;
onEnterFrame = function()
{
   if(oldframe !== frame)
   {
      gotoAndStop(frame);
      oldframe = frame;
      this.swapDepths(_root.M.frontwindow);
      greyness.aa = 0;
      greyness._alpha = 0;
   }
   bg._width = widt;
   bg._height = heigh;
   OEF();
   greyness._alpha += (greyness.aa - greyness._alpha) / 5;
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && _root.MenuAccess !== true)
   {
      _root.MenuAccess = true;
      ma = true;
   }
   else if(ma == true && !this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      ma = false;
      _root.MenuAccess = false;
   }
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      greyness.aa = 0;
      if(this.getDepth() !== _root.M.frontwindow)
      {
         this.swapDepths(_root.M.frontwindow);
      }
   }
   else
   {
      greyness.aa = 100;
   }
};
frame = undefined;
stop();
