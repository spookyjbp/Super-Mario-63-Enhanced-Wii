stop();
if(scale == undefined)
{
   scale = 2;
}
if(angle == undefined)
{
   angle = 0;
}
if(distance == undefined)
{
   distance = 50;
}
if(speed == undefined)
{
   speed = 5;
}
if(dotdistance == undefined)
{
   dotdistance = 20;
}
if(armscale == undefined)
{
   armscale = 0;
}
if(dotdistance < 360)
{
   i = 0;
   while(i < 360)
   {
      clip.attachMovie("Path dot","dot" + clip.getNextHighestDepth(),clip.getNextHighestDepth(),{_y:Math.cos(_root.angleToRadians(i)) * distance,_x:(- Math.sin(_root.angleToRadians(i))) * distance});
      i += dotdistance;
   }
}
if(armscale > 0)
{
   clip.attachMovie("PL circle arm","PLarm",clip.getNextHighestDepth(),{_y:Math.cos(_root.angleToRadians(angle)) * distance,_x:(- Math.sin(_root.angleToRadians(angle))) * distance,_rotation:angle - 180});
}
depth = _root.Course.BPlatforms.getNextHighestDepth();
_root.Course.BPlatforms.attachMovie("GreenPLcode","GreenPL" + depth,depth,{_x:_X,_y:_Y,scale:scale});
pl._y = Math.cos(_root.angleToRadians(angle)) * distance;
pl._x = (- Math.sin(_root.angleToRadians(angle))) * distance;
pl.gotoAndStop(scale);
newx = pl._x;
newy = pl._y;
oldx = newx;
oldy = newy;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      pl._y += diffy;
      angle += speed;
      newx = (- Math.sin(_root.angleToRadians(angle))) * distance;
      newy = Math.cos(_root.angleToRadians(angle)) * distance;
      diffy = newy - oldy;
      diffx = newx - oldx;
      oldx = newx;
      oldy = newy;
      pl._x += diffx;
      pl._y += diffy;
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 + distance * 2 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600 + distance * 2)
      {
         tellTarget(_root.Course.BPlatforms["GreenPL" + depth])
         {
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.diffy + 4) * _root.coursescale / 100 + _root.Course._y,false) && _root.Course.Char.yspeed > -3)
            {
               _root.Course.Char._x += this.diffx;
               _root.Course.Char._y += this.diffy;
               _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.3,1.1);
               if(landedon == false)
               {
                  _root.Course.Char.xspeed -= this.diffx / 2;
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
            _Y = this._y + this.pl._y;
            _X = this._x + this.pl._x;
         }
      }
      pl._y -= diffy;
      if(armscale > 0)
      {
         clip.PLarm._y = pl._y;
         clip.PLarm._x = pl._x;
         clip.PLarm._rotation = _root.radiansToAngle(Math.atan2(pl._y,pl._x)) + 90;
         clip.PLarm._yscale = armscale;
      }
   }
};
