if(trans == true)
{
   _xscale = (random(2) - 0.5) * 200;
   _yscale = (random(2) - 0.5) * 200;
}
else
{
   this.swapDepths(0);
   this.removeMovieClip();
}
