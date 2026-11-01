yspeed = 0;
xspeed = 0;
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
   if(hl == true && cl !== true)
   {
      this._xscale += (125 - this._xscale) / 4;
   }
   else if(cl !== true)
   {
      this._xscale += (100 - this._xscale) / 4;
   }
   this._yscale = this._xscale;
   xspeed = 0;
   yspeed = 0;
   i = 0;
   while(i < 2)
   {
      if(i == 0)
      {
         dist = Math.sqrt(Math.pow(this._x - _parent._xmouse,2) + Math.pow(this._y - _parent._ymouse,2));
         rot = Math.atan2(this._y - _parent._ymouse,this._x - _parent._xmouse);
      }
      else
      {
         rot = Math.atan2(this._y - _parent.no._y,this._x - _parent.no._x);
         dist = Math.sqrt(Math.pow(this._x - _parent.no._x,2) + Math.pow(this._y - _parent.no._y,2)) / 2;
      }
      if(i == 0)
      {
         dist = Math.max(dist,15);
      }
      else
      {
         dist = Math.max(dist,3);
      }
      xspeed += Math.cos(rot) * (200 / dist);
      yspeed += Math.sin(rot) * (200 / dist);
      if(hl !== true)
      {
         this._x += xspeed;
         this._y += yspeed;
      }
      else
      {
         this._x += xspeed / 2;
         this._y += yspeed / 2;
      }
      this._x = Math.min(225 - _width / 2,_X);
      this._x = Math.max(- (225 - _width / 2),_X);
      this._y = Math.min(150 - _height / 2,_Y);
      this._y = Math.max(- (150 - _height / 2),_Y);
      i++;
   }
};
onMouseDown = function()
{
   if(hl == true && cl !== true && _parent._currentframe == 10)
   {
      _parent.YesF();
      cl = true;
   }
};
