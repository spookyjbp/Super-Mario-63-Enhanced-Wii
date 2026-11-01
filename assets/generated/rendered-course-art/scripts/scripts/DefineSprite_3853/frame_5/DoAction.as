onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
      this._rotation += rspeed * (_alpha / 80);
      this._xscale -= 3;
      this._yscale -= 3;
      this._alpha -= alf / 12;
   }
   else
   {
      stop();
   }
};
