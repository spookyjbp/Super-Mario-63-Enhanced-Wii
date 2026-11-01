onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      this._rotation += rspeed * (_alpha / 80);
      this._xscale -= 6;
      this._yscale -= 6;
      this._alpha -= alf / 6;
      play();
   }
   else
   {
      stop();
   }
};
