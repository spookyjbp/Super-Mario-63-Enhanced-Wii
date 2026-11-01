alf = _alpha;
this._alpha = 0;
this._xscale = 50;
this._yscale = 50;
rspeed = (random(2) - 0.5) * 2 * (Math.random() * 5 + 2);
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      this._rotation += rspeed * 1.25;
      this._xscale += 25;
      this._yscale += 25;
      this._alpha += alf / 2;
      play();
   }
   else
   {
      stop();
   }
};
