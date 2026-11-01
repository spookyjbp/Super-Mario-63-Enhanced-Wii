onEnterFrame = function()
{
   if(_root.PauseGame !== true)
   {
      this._rotation += rspeed * (_alpha / 80);
      this._xscale *= 1.01;
      this._yscale *= 1.01;
      this._alpha -= alf / 14;
      play();
   }
   else
   {
      stop();
   }
};
