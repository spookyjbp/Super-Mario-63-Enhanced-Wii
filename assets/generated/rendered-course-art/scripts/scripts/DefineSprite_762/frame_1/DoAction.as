onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      sc = 120;
      gotoAndStop(3);
   }
   else
   {
      sc = 100;
      gotoAndStop(2);
   }
   if(_parent.Mode == "Off")
   {
      gotoAndStop(4);
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
};
onMouseDown = function()
{
   if(sc == 120)
   {
      _root.MuteBGMusic = false;
      _root.PlayMusicAndIntro();
      _parent.Mode = "Off";
   }
};
stop();
