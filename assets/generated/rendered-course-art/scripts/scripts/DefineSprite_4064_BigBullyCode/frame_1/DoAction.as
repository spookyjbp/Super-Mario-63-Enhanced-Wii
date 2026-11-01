if(scale == undefined)
{
   scale = 2;
}
if(Direction == undefined)
{
   Direction = 1;
}
_yscale = scale * 50;
_xscale = scale * 50 * Direction;
restartvars = function()
{
   deadcount = 0;
   xspeed = 0;
   yspeed = 0;
   hurt = false;
   frame = "idle";
   _xscale = scale * 50;
   tall = 25 * scale;
   wide = 15 * scale;
   stun = 0;
   HP = 3;
   if(jumpy == udefined)
   {
      jumpy = -19;
   }
   saveY = _Y;
   saveX = _X;
};
restartvars();
