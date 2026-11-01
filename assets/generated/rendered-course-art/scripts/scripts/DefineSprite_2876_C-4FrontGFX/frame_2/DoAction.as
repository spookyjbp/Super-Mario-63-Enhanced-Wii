s._xscale = _root.TotalStars * 100 / 64;
s._yscale = s._xscale;
s._alpha = _root.TotalStars * 75 / 64 + 25;
st._xscale = _root.TotalStars / 2 + 68;
st._yscale = st._xscale;
if(_root.Star[39] == false)
{
   _root.Course.BackGFX.s._alpha = 0;
   s._alpha = 0;
   st._alpha = 0;
}
stop();
