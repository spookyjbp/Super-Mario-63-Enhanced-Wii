wave = 0;
Y = this._y;
startscale = _yscale + 50;
flux = Math.round(startscale * 0.02) * 50 * 0.3;
onEnterFrame = function()
{
   wave += 0.05;
   this._yscale = startscale + Math.sin(wave) * flux;
};
