onEnterFrame = function()
{
   if(_root.BGquality == "LOW")
   {
      gotoAndStop(1);
   }
   if(_root.BGquality == "MEDIUM")
   {
      gotoAndStop(2);
   }
   if(_root.BGquality == "HIGH")
   {
      gotoAndStop(3);
   }
   if(_root.BGquality == "MEDIUM")
   {
      s._xscale = _root.TotalStars * 100 / 64 / 2 + 25;
      s._yscale = s._xscale;
      s._rotation += 2;
      s._alpha = (_root.TotalStars * 100 / 64 + 25) / 1.5 + 40;
   }
   if(_root.BGquality == "MEDIUM" || _root.BGquality == "HIGH")
   {
      st._xscale = (_root.TotalStars / 2 + 68) / 2;
      st._yscale = st._xscale;
   }
};
s.swapDepths(0);
s.removeMovieClip();
stop();
