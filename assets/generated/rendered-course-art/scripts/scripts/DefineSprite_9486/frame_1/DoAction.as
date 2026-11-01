ywave = random(90);
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
onEnterFrame = function()
{
   sc = 100;
   _xscale = _xscale + (sc - _xscale) / 4;
   _yscale = _xscale;
   txt._xscale += (sc - txt._xscale) / 4;
   txt._yscale = txt._xscale;
   ywave += 0.035;
   rwave += 0.025;
   this._y = Y + Math.sin(ywave) * 6;
   this._rotation = R + Math.sin(rwave) * _xscale / 20;
};
