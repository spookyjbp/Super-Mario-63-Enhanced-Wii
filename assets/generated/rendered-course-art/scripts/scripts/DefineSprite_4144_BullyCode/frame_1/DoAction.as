if(scale == undefined)
{
   scale = 1;
}
restartvars = function()
{
   deadcount = 0;
   xspeed = 0;
   yspeed = 0;
   hurt = false;
   frame = "idle";
   _xscale = scale * 100;
   tall = 25 * scale;
   wide = 15 * scale;
   stun = 0;
};
restartvars();
