ywave = 0;
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
onEnterFrame = function()
{
   if(_parent._currentframe >= 20 && _parent.Mode !== "copying" && _parent.Mode !== "deleting")
   {
      _alpha = _alpha + (100 - _alpha) / 5;
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
         txt._xscale += (125 - this._xscale) / 5;
         txt._yscale += (125 - this._yscale) / 4;
         ywave -= 0.025;
      }
      else if(cl !== true)
      {
         this._xscale += (100 - this._xscale) / 4;
         r -= (100 - this._xscale) / 10;
         txt._xscale += (100 - this._xscale) / 5;
         txt._yscale += (100 - this._yscale) / 4;
      }
      this._yscale = this._xscale;
      rwave += 0.025;
   }
   else if(_parent.Mode == "copying")
   {
      _alpha = _alpha + (50 - _alpha) / 5;
      this._xscale += (50 - this._xscale) / 4;
      _yscale = _xscale;
   }
};
onMouseDown = function()
{
   if(_parent._currentframe >= 20 && _parent.Mode !== "copying")
   {
      if(hl == true && cl !== true && _parent.clicked !== true)
      {
         if(_root["File" + _parent.HL + "TotalStars"] !== 0)
         {
            cl = true;
            rspeed = 0;
            scalespeed = 8;
            _parent.Mode = "deleting";
            _parent._parent.TransitionF("Delete");
         }
         else
         {
            _parent._parent.Message("Can\'t Delete an Empty File","red",false);
         }
      }
   }
};
