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
   walkspeed = 2;
}
if(airspeed == undefined)
{
   airspeed = 1;
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
   WalkDistance = 75;
}
if(IdleTime == undefined)
{
   IdleTime = 50;
}
if(randomWalkDistance == undefined)
{
   randomWalkDistance = 3;
}
if(randomIdleTime == undefined)
{
   randomIdleTime = 2;
}
restartvars = function()
{
   deadcount = 0;
   State = savestate;
   bounce = false;
   frame = "Idle";
   xspeed = 0;
   yspeed = 0;
   wide = 7 * scale;
   tall = 25 * scale;
   air = false;
   idle = true;
   _xscale = scale * 100;
   _yscale = scale * 100;
   rot = 0;
   rotspeed = 0;
   powerhit = false;
};
restartvars();
saveY = _Y;
saveX = _X;
if(_root.Course.coincountnum == undefined)
{
   _root.Course.coincountnum = 0;
}
else
{
   _root.Course.coincountnum++;
}
if(_root["coindata" + _root.playingcourse][0] == undefined)
{
   _root["coindata" + _root.playingcourse] = [""];
}
coinnum = _root.Course.coincountnum;
if(_root["coindata" + _root.playingcourse][coinnum] == 1 && _root["coindata" + _root.playingcourse][coinnum] !== undefined)
{
}
