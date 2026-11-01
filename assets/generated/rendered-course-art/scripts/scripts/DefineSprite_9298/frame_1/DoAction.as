awave = 90;
a2 = 0;
_alpha = 0;
anum = 1;
_xscale = 25;
_yscale = 25;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      hl = true;
   }
   else
   {
      hl = false;
   }
   if(hl == true && cl !== true)
   {
      a2 = Math.min(a2 + 3,50);
      _xscale = _xscale + (130 - _xscale) / 5;
      _yscale = _xscale;
   }
   else if(cl !== true)
   {
      _xscale = _xscale + (100 - _xscale) / 5;
      _yscale = _xscale;
      a2 = Math.max(a2 - 3,0);
   }
   else
   {
      _xscale = _xscale + 5;
      _yscale = _xscale;
   }
   if(anum > 20)
   {
      awave += 0.03;
      _alpha = Math.min(75 + Math.sin(awave) * 30 + a2,100);
   }
   else
   {
      _xscale = _xscale + (100 - _xscale) / 5;
      _yscale = _xscale;
      anum++;
      _alpha = _alpha + (100 - _alpha) / 5;
   }
};
onMouseDown = function()
{
   if(hl == true)
   {
      cl = true;
      _root.attachMovie("TitleScreenTransition","Transition" + _root.getNextHighestDepth(),_root.getNextHighestDepth(),{_x:225,_y:150,trans:true});
   }
};
