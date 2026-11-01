if(trans == true)
{
   _alpha = 0;
   onEnterFrame = function()
   {
      this._alpha += 5;
      if(_alpha >= 100)
      {
         _alpha = 100;
         _root.m.gotoAndStop("Refresh");
      }
   };
}
else
{
   this.swapDepths(0);
   this.removeMovieClip();
}
file = _root.m.filem.HL;
txtt = "Are you sure you want to Delete File " + file + "?";
YesF = function()
{
   _root.m.gotoAndStop("Story");
   _root.m.SpecialDeleteFile(file);
   play();
   onEnterFrame = function()
   {
      this._alpha -= 5;
      no._alpha -= 2;
      no._xscale -= 2;
      no._yscale -= 2;
      yes._xscale += 2;
      yes._yscale += 2;
      if(_alpha <= 0)
      {
         _alpha = 0;
         this.removeMovieClip();
      }
   };
};
NoF = function()
{
   YesF = null;
   _root.m.gotoAndStop("Story");
   gotoAndPlay(40);
   onEnterFrame = function()
   {
      yes._alpha -= 2;
      yes._xscale -= 2;
      yes._yscale -= 2;
      no._xscale += 2;
      no._yscale += 2;
      this._alpha -= 5;
      if(_alpha <= 0)
      {
         _alpha = 0;
         this.removeMovieClip();
      }
   };
};
