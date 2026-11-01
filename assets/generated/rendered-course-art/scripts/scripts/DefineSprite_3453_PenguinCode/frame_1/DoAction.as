if(scale == undefined)
{
   scale = 1;
}
if(State == undefined)
{
   State = "Walk";
}
if(walkspeed == undefined)
{
   walkspeed = 1.5;
}
if(frame == undefined)
{
   frame = "Walk";
}
movecount = 0;
sliding = false;
if(movedirection == undefined)
{
   randomnum = random(2);
   if(randomnum == 0)
   {
      movedirection = "right";
   }
   else
   {
      movedirection = "left";
   }
}
else
{
   randomnum = 0;
}
if(WalkDistance == undefined)
{
   WalkDistance = 50;
}
if(shellspeed == undefined)
{
   shellspeed = 10;
}
flyingcoswave = 0;
randomturncount = 50;
turncount = 0;
randomnum = 0;
if(randomIdleTime == undefined)
{
   randomIdleTime = 3;
}
if(randomWalkDistance == undefined)
{
   randomWalkDistance = 5;
}
if(IdleTime == undefined)
{
   IdleTime = 50;
}
frame = "Idle";
xspeed = 0;
yspeed = 0;
wide = 12 * scale;
tall = 25 * scale;
idle = false;
air = false;
rotspeed = 0;
tumble = false;
startY = _Y;
rot = 0;
_xscale = scale * 100;
_yscale = scale * 100;
