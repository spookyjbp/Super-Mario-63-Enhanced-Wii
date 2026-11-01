num = 4;
onEnterFrame = function()
{
   this._alpha += (aa - _alpha) / num;
   if(aa == 0 && this._alpha <= 2)
   {
      this.removeMovieClip();
   }
   if(_root.M.Window1.frame !== "Reset")
   {
      aa = -5;
   }
   if(this._alpha >= 98)
   {
      _root.LDDeleteF();
      aa = -10;
      num = 30;
   }
};
