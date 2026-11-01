alf = _alpha;
this._alpha = 0;
this._xscale *= 0.5;
this._yscale *= 0.5;
rspeed = (random(2) - 0.5) * 2 * (Math.random() * 3 + 1);
onEnterFrame = function()
{
   if(_root.PauseGame !== true)
   {
      this._rotation += rspeed * 1.25;
      this._xscale *= 1.2;
      this._yscale *= 1.2;
      this._alpha += alf / 5;
      play();
   }
   else
   {
      stop();
   }
};
