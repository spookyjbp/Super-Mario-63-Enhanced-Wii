if(frame == undefined)
{
   frame = "flying";
}
if(frame == "run")
{
   blink = 0;
   if(xspeed == undefined)
   {
      xspeed = 0;
   }
   if(yspeed == undefined)
   {
      yspeed = 0;
   }
}
if(flyxspeed == undefined)
{
   flyxspeed = 1.5 + Math.random() * 0.5;
}
if(flyyspeed == undefined)
{
   flyyspeed = -0.1 - Math.random() * 0.2;
}
if(runspeed == undefined)
{
   runspeed = 2 + Math.random() * 1.5;
}
air = true;
if(frame == "flying")
{
   clip.clip.gotoAndPlay(random(18) + 1);
}
wide = 10;
tall = 7;
saveX = _X;
saveY = _Y;
saveframe = frame;
restartvars = function()
{
   _X = saveX;
   _Y = saveY;
   frame = saveframe;
   air = true;
   blink = 0;
};
