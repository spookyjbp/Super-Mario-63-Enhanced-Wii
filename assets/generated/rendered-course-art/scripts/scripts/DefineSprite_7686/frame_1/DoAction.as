stop();
if(DirectionX == undefined)
{
   DirectionX = "Right";
}
if(DirectionY == undefined)
{
   DirectionY = "Up";
}
if(distanceX == undefined)
{
   distanceX = 0;
}
if(distanceY == undefined)
{
   distanceY = 0;
}
if(accelX == undefined)
{
   accelX = 0;
}
if(accelY == undefined)
{
   accelY = 0;
}
if(speedX == undefined)
{
   speedX = 0;
}
if(speedY == undefined)
{
   speedY = 0;
}
if(Xoffstart == undefined)
{
   Xoffstart = 0;
}
if(Yoffstart == undefined)
{
   Yoffstart = 0;
}
if(DirectionY == "Up")
{
   _Y = _Y - speedY;
}
else
{
   _Y = _Y + speedY;
}
if(DirectionY == "Right")
{
   _X = _X - speedX;
}
else
{
   _X = _X + speedX;
}
if(scale == undefined)
{
   scale = 100;
}
if(blocktype == undefined)
{
   blocktype = 1;
}
if(angleoffset == undefined)
{
   angleoffset = 0;
}
if(rotamount == undefined)
{
   rotamount = 120;
}
if(rotdirection == undefined)
{
   rotdirection = "Left";
}
if(rotspeed == undefined)
{
   rotspeed = 3;
}
if(rotwait == undefined)
{
   rotwait = 92;
}
if(quake == undefined && rotwait > 20)
{
   quake = true;
}
else
{
   quake = true;
}
_yscale = scale;
_xscale = scale;
gotoAndStop(blocktype);
_rotation = angleoffset;
count = 0;
spincount = 0;
onEnterFrame = function()
{
   count++;
   if(count > rotwait)
   {
      spincount++;
      if(spincount * rotspeed <= rotamount)
      {
         if(this.rotdirection == "Right")
         {
            _rotation = _rotation + this.rotspeed;
         }
         if(this.rotdirection == "Left")
         {
            _rotation = _rotation - this.rotspeed;
         }
      }
      else
      {
         count = 0;
         spincount = 0;
         _rotation = _rotation - this.angleoffset;
         _rotation = Math.round(_rotation / rotamount) * rotamount + angleoffset;
      }
   }
   if(count + 20 > this.rotwait && count < this.rotwait)
   {
      _rotation = _rotation - this.angleoffset;
      _rotation = Math.round(_rotation / this.rotamount) * this.rotamount + this.angleoffset;
      _rotation = _rotation + (Math.random() * 5 - 2.5);
   }
};
