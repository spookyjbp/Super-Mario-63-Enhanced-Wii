scaly = 100;
qual = "HIGH";
onEnterFrame = function()
{
   if(hl == true)
   {
      _parent.qual = qual;
   }
   else if(_parent.qual == qual)
   {
      _parent.qual = undefined;
   }
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      hl = true;
   }
   else
   {
      hl = false;
   }
   if(hl == true)
   {
      _xscale = _xscale + (scaly + 25 - _xscale) / 5;
   }
   else
   {
      _xscale = _xscale + (scaly - _xscale) / 5;
   }
   if(_parent.realqual == qual)
   {
      scaly += (125 - scaly) / 5;
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
      scaly += (100 - scaly) / 5;
   }
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      scaly = 175;
   }
};
stop();
