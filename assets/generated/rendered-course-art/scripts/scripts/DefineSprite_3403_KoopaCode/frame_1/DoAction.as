if(scale == undefined)
{
   scale = 1;
}
if(State == undefined)
{
   State = "Walk";
}
savestate = State;
if(walkspeed == undefined)
{
   walkspeed = 3;
}
if(frame == undefined)
{
   frame = "Walk";
}
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
   WalkDistance = 100;
}
if(shellspeed == undefined)
{
   shellspeed = 10;
}
if(underwater == undefined)
{
   underwater = false;
}
restartvars = function()
{
   State = savestate;
   frame = "Idle";
   xspeed = 0;
   yspeed = 0;
   wide = 7 * scale;
   tall = 25 * scale;
   air = false;
   _xscale = scale * 100;
   _yscale = scale * 100;
};
restartvars();
saveY = _Y;
saveX = _X;
