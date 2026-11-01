xspeed = 0;
yspeed = 0;
rot = Math.random() * 3.141592653589793 * 2 - 3.141592653589793;
clip._rotation = Math.random() * 360;
rotspeed = Math.random() * 30 - 15;
speed = 17 + Math.random() * 2;
trail._rotation = rot * 180 / 3.141592653589793 + 90;
trail._yscale = 25;
clip3._visible = false;
clip2._visible = false;
clip._x += Math.cos(rot) * speed / 2;
clip._y += Math.sin(rot) * speed / 2;
count = 0;
onEnterFrame = function()
{
   count++;
   if(count == 2)
   {
      clip2._visible = true;
   }
   if(count == 3)
   {
      clip3._visible = true;
   }
   clip3._rotation = clip2._rotation;
   clip3._x = clip2._x;
   clip3._y = clip2._y;
   clip2._rotation = clip._rotation;
   clip2._x = clip._x;
   clip2._y = clip._y;
   clip._rotation += rotspeed;
   clip._x += Math.cos(rot) * speed;
   clip._y += Math.sin(rot) * speed;
   trail._x = clip._x;
   trail._y = clip._y;
   if(Math.abs(clip._x) > 300)
   {
      this.swapDepths(0);
      this.removeMovieClip();
   }
   if(Math.abs(clip._y) > 300)
   {
      this.swapDepths(0);
      this.removeMovieClip();
   }
   trail._xscale += (50 - trail._xscale) / 10;
   trail._yscale += (150 - trail._yscale) / 10;
};
