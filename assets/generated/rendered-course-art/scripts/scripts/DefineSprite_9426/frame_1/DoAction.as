ywave = 0;
rwave = random(90);
Y = this._y;
R = _rotation;
r = 0;
onEnterFrame = function()
{
   if(_parent._currentframe >= 20 && _root.m.MenuRedTrans._x == undefined)
   {
      if(_parent.Mode == "copying")
      {
         gotoAndStop(3);
      }
      else
      {
         gotoAndStop(2);
      }
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
      this._y = Y + Math.sin(ywave) * 3;
   }
};
onMouseDown = function()
{
   if(_parent._currentframe >= 20 && _root.m.MenuRedTrans._x == undefined)
   {
      if(hl == true && cl !== true && _parent.clicked !== true)
      {
         if(_root["File" + _parent.HL + "TotalStars"] > 0)
         {
            _parent.Mode = "copying";
         }
         else if(_root["File" + _parent.HL + "TotalStars"] == 0)
         {
            _parent._parent.Message("Can\'t Copy an Empty File!","red",false);
         }
         else
         {
            _parent._parent.Message("Can\'t Copy nothing!","red",false);
         }
      }
   }
};
gotoAndStop(2);
