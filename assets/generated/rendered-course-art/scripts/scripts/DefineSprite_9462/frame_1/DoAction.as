onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && _parent.Mode !== "none")
   {
      sc = 105;
      gotoAndStop(4);
   }
   else
   {
      if(_parent.Mode == "none")
      {
         gotoAndStop(2);
      }
      else
      {
         gotoAndStop(3);
      }
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 105)
   {
      _xscale = 130;
      _yscale = 130;
      _parent.Mode = "none";
      _parent.StopSong();
   }
};
