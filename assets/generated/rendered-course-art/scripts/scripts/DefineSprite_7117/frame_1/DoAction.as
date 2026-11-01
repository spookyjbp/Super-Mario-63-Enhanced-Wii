if(speed == undefined)
{
   speed = 1;
}
if(frame == undefined)
{
   frame = 1;
}
speed *= _root.Course.BackGFX.clockspeed;
if(frame !== 3)
{
   onEnterFrame = function()
   {
      _rotation = _rotation + speed;
   };
}
gotoAndStop(frame);
