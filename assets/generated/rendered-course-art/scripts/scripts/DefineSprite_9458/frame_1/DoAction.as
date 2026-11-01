onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true))
   {
      sc = 105;
      gotoAndStop(3);
   }
   else
   {
      if(_parent.Mode == "play" && _parent._parent.Songplaying == _parent._parent.Songnum)
      {
         gotoAndStop(4);
         sc = 100;
      }
      else
      {
         gotoAndStop(2);
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
      _parent.Mode = "play";
      _parent._parent.Songplaying = _parent._parent.Songnum;
      _parent.PlaySong();
   }
};
