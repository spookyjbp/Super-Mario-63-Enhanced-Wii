al = 50;
sc = 75;
onEnterFrame = function()
{
   if(this.hitTest(_root._xmouse,_root._ymouse,false))
   {
      al = 100;
      sc = 100;
   }
   else
   {
      al = 50;
      sc = 75;
   }
   _xscale = _xscale + (sc - _xscale) / 2;
   _yscale = _xscale;
   _alpha = _alpha + (al - _alpha) / 2;
};
onMouseDown = function()
{
   if(sc == 100)
   {
      if(LOW.sc !== 120 && MEDIUM.sc !== 120 && HIGH.sc !== 120)
      {
         _xscale = 110;
         _yscale = 110;
         if(_root.BGquality == "LOW")
         {
            _root.BGquality = "MEDIUM";
         }
         else if(_root.BGquality == "MEDIUM")
         {
            _root.BGquality = "HIGH";
         }
         else
         {
            _root.BGquality = "LOW";
         }
      }
   }
};
