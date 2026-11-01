_xscale = 100;
_yscale = 100;
if(distance == undefined)
{
   distance = 150;
}
if(angle == undefined)
{
   angle = 50;
}
if(thickness == undefined)
{
   thickness = 20;
}
if(topsize == undefined)
{
   topsize = 35;
}
if(basesize == undefined)
{
   basesize = 200;
}
if(speed == undefined)
{
   speed = 1000;
}
if(offstart == undefined)
{
   offstart = 0;
}
if(graphic == undefined)
{
   graphic = false;
}
if(graphic == false)
{
   depth = _root.Course.BPlatforms.getNextHighestDepth();
   _root.Course.BPlatforms.attachMovie("SwingingShipGround","SwingingShipGround" + depth,depth,{_x:_X,_y:_Y});
}
rotspeed = 0;
count = 0;
while(count++ < offstart)
{
   rotspeed += (- angle) / speed;
   angle += rotspeed;
}
base._yscale = topsize;
base._xscale = topsize;
arm._xscale = thickness;
arm._yscale = distance;
arm._rotation = angle;
bell._yscale = basesize;
bell._xscale = basesize;
bell._y = Math.cos(_root.angleToRadians(angle)) * distance;
bell._x = (- Math.sin(_root.angleToRadians(angle))) * distance;
bell._rotation = angle;
newx = bell._x;
oldx = bell._x;
newy = bell._y;
oldy = bell._y;
mariodistance = 0;
changed = false;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      b2._x = bell._x;
      b2._y = bell._y;
      b5._x = b2._x + Math.cos(3.141592653589793 * angle / 180) * mariodistance;
      b5._y = b2._y + Math.sin(3.141592653589793 * angle / 180) * mariodistance;
      newx = b5._x;
      newy = b5._y;
      diffy = newy - oldy;
      diffx = newx - oldx;
      oldx = newx;
      oldy = newy;
      if(changed == true)
      {
         diffy = 0;
         diffx = 0;
         newx = b5._x;
         newy = b5._y;
         oldx = b5._x;
         oldy = b5._y;
      }
      base._yscale = topsize;
      base._xscale = topsize;
      arm._xscale = thickness;
      arm._yscale = distance;
      arm._rotation = angle;
      bell._yscale = basesize;
      bell._xscale = basesize;
      bell._y = Math.cos(_root.angleToRadians(angle)) * distance;
      bell._x = (- Math.sin(_root.angleToRadians(angle))) * distance;
      bell._rotation = angle;
      if(graphic == false)
      {
         tellTarget(_root.Course.BPlatforms["SwingingShipGround" + depth])
         {
            _Y = this._y + Math.cos(_root.angleToRadians(this.angle)) * this.distance;
            _X = this._x - Math.sin(_root.angleToRadians(this.angle)) * this.distance;
            _rotation = this.angle;
            _yscale = this.basesize;
            _xscale = this.basesize;
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Char.air == false)
            {
               if(this.changed == false && landedon == true)
               {
                  _root.Course.Char._x += this.diffx;
                  _root.Course.Char._y += this.diffY;
                  charYdistance = _root.Course.Char._y - (this._y + _root.Course.BackGFX._y + this.b5._y);
                  charXdistance = _root.Course.Char._x - (this._x + _root.Course.BackGFX._x + this.b5._x);
                  if(Math.abs(charYdistance) + Math.abs(charXdistance) < this.distance / 2)
                  {
                     _root.Course.Char._x = this._x + _root.Course.BackGFX._x + this.b5._x;
                     _root.Course.Char._y = this._y + _root.Course.BackGFX._y + this.b5._y;
                  }
                  this.mariodistance += _root.Course.Char.xspeed;
               }
               this.changed = false;
               while(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true) && !hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - 3) * _root.coursescale / 100 + _root.Course._y,true))
               {
                  _root.Course.Char._y++;
               }
               if(landedon == false)
               {
                  _root.Course.Char.xspeed -= this.diffx;
                  this.b5._x = _root.Course.Char._x - _root.Course.BackGFX._x - this._x;
                  this.b5._y = _root.Course.Char._y - _root.Course.BackGFX._y - this._y;
                  this.xdistance = this.b5._x - this.b2._x;
                  this.ydistance = this.b5._y - this.b2._y;
                  this.rot = Math.atan2(this.ydistance,this.xdistance);
                  this.mariodistance = Math.sqrt(Math.pow(this.xdistance,2) + Math.pow(this.ydistance,2));
                  this.mariodistance = Math.min(this.mariodistance,this.distance / 1.75);
                  if(this.xdistance < 0)
                  {
                     this.mariodistance *= -1;
                  }
                  this.changed = true;
                  b5._x = b2._x + Math.cos(3.141592653589793 * angle / 180) * mariodistance;
                  b5._y = b2._y + Math.sin(3.141592653589793 * angle / 180) * mariodistance;
               }
               landedon = true;
            }
            else
            {
               if(landedon == true)
               {
                  _root.Course.Char.xspeed += this.diffx / 2;
                  _root.Course.Char.yspeed += this.diffy / 2;
               }
               landedon = false;
            }
         }
      }
      rotspeed += (- angle) / speed;
      angle += rotspeed;
   }
};
