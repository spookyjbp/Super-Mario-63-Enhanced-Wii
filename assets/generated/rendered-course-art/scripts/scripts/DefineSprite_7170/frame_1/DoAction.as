if(speed == undefined)
{
   speed = 1;
   speed *= 4;
}
if(frame == undefined)
{
   frame = 1;
}
onEnterFrame = function()
{
   frame += speed;
   if(frame < 1)
   {
      frame += 32;
   }
   else if(frame > 32)
   {
      frame -= 32;
   }
   gotoAndStop(Math.round(frame));
};
stop();
