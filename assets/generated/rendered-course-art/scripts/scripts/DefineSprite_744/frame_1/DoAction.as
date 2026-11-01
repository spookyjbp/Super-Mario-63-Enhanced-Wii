onEnterFrame = function()
{
   if(_root.Fluddpow == "T" || this.hitTest(_root._xmouse,_root._ymouse,true) && _root.FluddArray[_root.Playinglevel][3] == true)
   {
      sc = 115;
   }
   else
   {
      sc = 100;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   if(Math.abs(sc - _xscale) < 1)
   {
      _xscale = sc;
   }
   if(_root.FluddArray[_root.Playinglevel][3] == true)
   {
      gotoAndStop(1);
   }
   else
   {
      gotoAndStop(2);
   }
};
onMouseDown = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,true) && (_root.FluddArray[_root.Playinglevel][3] == true || _root.SaveFluddT == true) && _root.CanSwitchFludd !== false)
   {
      _root.Fluddpow = "T";
   }
};
