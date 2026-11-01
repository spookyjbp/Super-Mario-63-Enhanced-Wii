stop();
if(speed == undefined)
{
   speed = 3;
}
if(scale == undefined)
{
   scale = 1;
}
pl.gotoAndStop(scale);
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("FlippingPLGround","FlippingPLGround" + depth,depth,{_x:_X,_y:_Y});
tellTarget(_root.Course.Platforms["FlippingPLGround" + depth])
{
   _xscale = this.scale * 100;
   _yscale = this.scale * 100;
}
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      _rotation = _rotation + speed;
      tellTarget(_root.Course.Platforms["FlippingPLGround" + depth])
      {
         ydistance = _Y + _parent._y - _root.Course.Char._y;
         xdistance = _X + _parent._x - _root.Course.Char._x;
         distance = Math.sqrt(Math.pow(xdistance,2) + Math.pow(ydistance,2));
         rot = Math.atan2(ydistance,xdistance);
         xdiff = Math.cos(rot) * distance - Math.cos(rot + _root.angleToRadians(this.speed)) * distance;
         ydiff = Math.sin(rot) * distance - Math.sin(rot + _root.angleToRadians(this.speed)) * distance;
         if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true))
         {
            _root.Course.Char._x += xdiff;
            _root.Course.Char._y += ydiff;
            if(landedon == false)
            {
               _root.Course.Char.xspeed -= xdiff;
            }
            landedon = true;
         }
         else
         {
            if(landedon == true)
            {
               _root.Course.Char.xspeed += xdiff;
               _root.Course.Char.yspeed += ydiff;
            }
            landedon = false;
         }
         _rotation = this._rotation;
      }
   }
};
