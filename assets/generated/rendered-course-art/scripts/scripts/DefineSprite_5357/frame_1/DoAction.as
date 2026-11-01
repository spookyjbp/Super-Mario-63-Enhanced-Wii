yspeed = 0;
startY = _Y;
frame = "idle";
if(mariowait == undefined)
{
   mariowait = false;
}
startcounting = false;
if(waittime == undefined)
{
   waittime = _root.framerate * 3;
}
if(count == undefined)
{
   count = 0;
}
if(groundwaittime == undefined)
{
   groundwaittime = _root.framerate * 0.5;
}
if(fallspeed == undefined)
{
   fallspeed = 7;
}
if(fallaccel == undefined)
{
   fallaccel = 1;
}
if(riseaccel == undefined)
{
   riseaccel = 0.1;
}
if(risespeed == undefined)
{
   risespeed = 3;
}
if(Xrange == undefined)
{
   Xrange = 30;
}
riseaccel = 0.1;
risespeed = 3;
ycutoff = 0;
yspeed = risespeed;
while(yspeed >= riseaccel)
{
   yspeed -= riseaccel;
   ycutoff += yspeed;
}
yspeed = 0;
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("ThwompGround","ThwompGround" + depth,depth,{_x:_X,_y:_Y,scale:scale});
depth2 = _root.Course.BPlatforms.getNextHighestDepth();
_root.Course.BPlatforms.attachMovie("ThwompPlatform","ThwompPlatform" + depth2,depth2,{_x:_X,_y:_Y,scale:scale});
