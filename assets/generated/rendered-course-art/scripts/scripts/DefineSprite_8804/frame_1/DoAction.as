gotoAndStop(_parent.ydirection);
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      hl = true;
      sc = 120;
   }
   else
   {
      hl = false;
      sc = 100;
   }
   gotoAndStop(_parent.ydirection);
   _xscale = _xscale + (sc - _xscale) / 3;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(hl == true)
   {
      if(_parent.ydirection == "Up")
      {
         _parent.ydirection = "Down";
      }
      else if(_parent.ydirection == "Down")
      {
         _parent.ydirection = "none";
      }
      else
      {
         _parent.yspeednum = 2;
         _parent.ylengthnum = 40;
         _parent.yoffsetnum = 0;
         _parent.yaccel = 0;
         _parent.ydirection = "Up";
      }
   }
};
