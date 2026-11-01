ywave = random(90);
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
onEnterFrame = function()
{
   if(bg.hitTest(_root._xmouse,_root._ymouse,true) && _parent.clicked !== true)
   {
      hl = true;
   }
   else
   {
      hl = false;
   }
   if(cl == true)
   {
      rspeed += 1;
      r -= rspeed;
      rspeed *= 0.95;
      this._xscale += scalespeed;
      scalespeed -= 1;
      if(this._xscale < 60)
      {
         scalespeed += 1;
         scalespeed *= 0.7;
      }
   }
   if(hl == true && cl !== true)
   {
      this._xscale += (125 - this._xscale) / 4;
      r -= (125 - this._xscale) / 10;
      txt._xscale += (130 - txt._xscale) / 5;
      txt._yscale += (125 - txt._yscale) / 4;
      ywave -= 0.035;
   }
   else if(cl !== true)
   {
      this._xscale += (100 - this._xscale) / 4;
      r -= (100 - this._xscale) / 10;
      txt._xscale += (100 - txt._xscale) / 5;
      txt._yscale += (100 - txt._yscale) / 4;
   }
   this._yscale = this._xscale;
   ywave += 0.035;
   rwave += 0.025;
   this._y = Y + Math.sin(ywave) * 6;
   this._rotation = R + r + Math.sin(rwave) * _xscale / 20;
};
onMouseDown = function()
{
   if(hl == true && cl !== true && _parent.clicked !== true)
   {
      cl = true;
      rspeed = 0;
      scalespeed = 8;
      _parent.clicked = true;
      _parent.TransitionF("Extras");
   }
};
