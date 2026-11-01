ywave = random(90);
Y = this._y;
r = 0;
if(_parent._currentframe > 20)
{
   cl = true;
   rewinding = true;
   _alpha = 0;
   this._xscale = 300;
   this._yscale = 300;
}
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
   if(cl == true && rewinding !== true)
   {
      this._xscale += (300 - this._xscale) / 10;
      txt._xscale += (125 - txt._xscale) / 5;
      txt._yscale += (125 - txt._yscale) / 4;
      _alpha = _alpha - 5;
      if(_alpha <= 0)
      {
         StopCoding();
      }
   }
   if(cl == true && rewinding == true)
   {
      this._xscale += (100 - this._xscale) / 10;
      txt._xscale += (100 - txt._xscale) / 5;
      txt._yscale += (100 - txt._yscale) / 4;
      _alpha = _alpha + 5;
      if(_alpha >= 100)
      {
         rewinding = false;
         cl = false;
      }
   }
   if(hl == true && cl !== true)
   {
      this._xscale += (125 - this._xscale) / 4;
      txt._xscale += (125 - txt._xscale) / 5;
      txt._yscale += (125 - txt._yscale) / 4;
      ywave -= 0.035;
   }
   else if(cl !== true)
   {
      this._xscale += (100 - this._xscale) / 4;
      txt._xscale += (100 - txt._xscale) / 5;
      txt._yscale += (100 - txt._yscale) / 4;
   }
   this._yscale = this._xscale;
   ywave += 0.035;
   this._y = Y + Math.sin(ywave) * 4;
};
onMouseDown = function()
{
   if(hl == true && cl !== true && _root.m.clicked !== true)
   {
      _root.m.Mode = "Quality";
      cl = true;
      _parent.HL = "defined";
      onMouseDown = null;
   }
};
StopCoding = function()
{
   onEnterFrame = null;
   onMouseDown = null;
   gotoAndStop(2);
};
stop();
