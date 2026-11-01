if(trans == true)
{
   _alpha = 0;
   onEnterFrame = function()
   {
      this._alpha += 4;
      if(_alpha >= 100)
      {
         _alpha = 100;
         _root.StopBGsong();
         _root.nextFrame();
         this.removeMovieClip();
      }
   };
}
else
{
   this.swapDepths(0);
   this.removeMovieClip();
}
