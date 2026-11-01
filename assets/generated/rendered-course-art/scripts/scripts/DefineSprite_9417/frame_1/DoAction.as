ywave = random(90);
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
StartCoding = function()
{
   onEnterFrame = function()
   {
      if(bg.hitTest(_root._xmouse,_root._ymouse,true) && _root.m.clicked !== true)
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
         txt._xscale += (125 - this._xscale) / 5;
         txt._yscale += (125 - this._yscale) / 4;
         ywave -= 0.035;
      }
      else if(cl !== true)
      {
         this._xscale += (100 - this._xscale) / 4;
         r -= (100 - this._xscale) / 10;
         txt._xscale += (100 - this._xscale) / 5;
         txt._yscale += (100 - this._yscale) / 4;
      }
      this._yscale = this._xscale;
      ywave += 0.035;
      rwave += 0.025;
      this._y = Y + Math.sin(ywave) * 4;
      this._rotation = R + r + Math.sin(rwave) * _xscale / 50;
   };
   onMouseDown = function()
   {
      if(hl == true && cl !== true && _root.m.clicked !== true)
      {
         cl = true;
         rspeed = 0;
         scalespeed = 8;
         _root.m.clicked = true;
         _root.m.TransitionF("Start");
      }
   };
};
StartCoding();
StopCoding = function()
{
   onEnterFrame = function()
   {
      _rotation = _rotation + 1;
      this._xscale += 5;
      this._yscale = this._xscale;
      _alpha = _alpha - 9;
      if(_alpha <= 0)
      {
         _alpha = 0;
         gotoAndStop(3);
         onEnterFrame = null;
      }
   };
   onMouseDown = null;
};
RestartCoding = function()
{
   gotoAndStop(2);
   onEnterFrame = function()
   {
      _rotation = _rotation - 1;
      this._xscale -= 5;
      this._yscale = this._xscale;
      _alpha = _alpha + 9;
      if(_alpha >= 100)
      {
         _alpha = 100;
         StartCoding();
      }
   };
   onMouseDown = null;
};
gotoAndStop(2);
