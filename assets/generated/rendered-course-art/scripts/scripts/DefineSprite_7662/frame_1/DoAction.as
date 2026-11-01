stop();
txt = "Tiles";
rev = false;
onEnterFrame = function()
{
   if((this.hitTest(_root._xmouse,_root._ymouse,false) && !_root.M.Window1.hitTest(_root._xmouse,_root._ymouse,false) || _parent.cl == txt) && rev == false)
   {
      if(_currentframe < 3)
      {
         nextFrame();
         if(_parent.cl !== "")
         {
            _parent.cl = txt;
         }
      }
      if(_parent.cl == txt)
      {
         if(_root.M.Bar.getDepth() !== _root.M.frontwindow)
         {
            _root.M.Bar.swapDepths(_root.M.frontwindow);
         }
         if(_currentframe < 8)
         {
            nextFrame();
         }
      }
   }
   else if(_currentframe > 1)
   {
      prevFrame();
   }
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false) && !_root.M.Window1.hitTest(_root._xmouse,_root._ymouse,false))
   {
      _parent.cl = txt;
   }
   else if(_parent.cl == txt)
   {
      _parent.cl = "";
   }
};
