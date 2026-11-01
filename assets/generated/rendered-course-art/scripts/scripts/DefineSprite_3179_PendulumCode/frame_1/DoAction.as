_xscale = 100;
_yscale = 100;
if(distance == undefined)
{
   distance = 150;
}
if(angle == undefined)
{
   angle = 70;
}
if(thickness == undefined)
{
   thickness = 20;
}
if(bellsize == undefined)
{
   bellsize = 50;
}
if(basesize == undefined)
{
   basesize = 30;
}
if(speed == undefined)
{
   speed = 800;
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
   depth = _root.Course.Platforms.getNextHighestDepth();
   _root.Course.Platforms.attachMovie("Pendelum Ground","PendelumGround" + depth,depth,{_x:_X,_y:_Y});
}
rotspeed = 0;
count = 0;
while(count++ < offstart)
{
   rotspeed += (- angle) / speed;
   angle += rotspeed;
}
base._yscale = basesize;
base._xscale = basesize;
arm._xscale = thickness;
arm._yscale = distance;
arm._rotation = angle;
bell._yscale = bellsize;
bell._xscale = bellsize;
bell._y = Math.cos(_root.angleToRadians(angle)) * distance;
bell._x = (- Math.sin(_root.angleToRadians(angle))) * distance;
newx = bell._x;
oldx = bell._x;
newy = bell._y;
oldy = bell._y;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      newx = bell._x;
      newy = bell._y;
      diffy = newy - oldy;
      diffx = newx - oldx;
      oldx = newx;
      oldy = newy;
      base._yscale = basesize;
      base._xscale = basesize;
      arm._xscale = thickness;
      arm._yscale = distance;
      arm._rotation = angle;
      bell._yscale = bellsize;
      bell._xscale = bellsize;
      bell._y = Math.cos(_root.angleToRadians(angle)) * distance;
      bell._x = (- Math.sin(_root.angleToRadians(angle))) * distance;
      if(graphic == false)
      {
         tellTarget(_root.Course.Platforms["PendelumGround" + depth])
         {
            _Y = this._y + Math.cos(_root.angleToRadians(this.angle)) * this.distance;
            _X = this._x - Math.sin(_root.angleToRadians(this.angle)) * this.distance;
            _yscale = this.bellsize;
            _xscale = this.bellsize;
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true))
            {
               _root.Course.Char._x += this.diffx;
               _root.Course.Char._y += this.diffY;
               _root.Course.Char._x += this.rotspeed * this.bellsize / 100;
               _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.3,1.1);
               if(landedon == false)
               {
                  _root.Course.Char.xspeed -= this.diffx;
               }
               landedon = true;
            }
            else
            {
               if(landedon == true)
               {
                  _root.Course.Char.xspeed += this.diffx;
                  _root.Course.Char.yspeed += this.diffy;
               }
               landedon = false;
            }
         }
      }
      rotspeed += (- angle) / speed;
      angle += rotspeed;
   }
};
