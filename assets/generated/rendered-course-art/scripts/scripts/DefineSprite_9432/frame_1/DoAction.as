onEnterFrame = function()
{
   if(_parent._currentframe >= 22 && _parent.Mode !== "deleting")
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
         txt._xscale += (125 - this._xscale) / 5;
         txt._yscale += (125 - this._yscale) / 4;
      }
      else if(cl !== true)
      {
         this._xscale += (100 - this._xscale) / 4;
         r -= (100 - this._xscale) / 10;
         txt._xscale += (100 - this._xscale) / 5;
         txt._yscale += (100 - this._yscale) / 4;
      }
      this._yscale = this._xscale;
   }
};
onMouseDown = function()
{
   if(_parent._currentframe >= 22 && _parent.Mode !== "deleting")
   {
      if(hl == true && cl !== true && _parent.clicked !== true)
      {
         cl = true;
         rspeed = 0;
         scalespeed = 8;
         _parent.HL = undefined;
      }
   }
};
