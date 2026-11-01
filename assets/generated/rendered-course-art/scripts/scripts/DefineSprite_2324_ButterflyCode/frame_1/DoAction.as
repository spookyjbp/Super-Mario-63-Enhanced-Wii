frame = random(6) + 1;
diff = random(5);
count = 0;
count2 = 0;
scale = random(40) + 80;
maxxspeed = diff / 5 + 0.4;
xspeed = maxxspeed;
waittime = random(25) + 50;
num = random(2) + 1;
if(num == 1)
{
   movedir = "right";
}
else
{
   movedir = "left";
}
_yscale = scale;
_xscale = scale;
wave = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(_root.BGquality == "LOW" || _root.Detailquality == "LOW")
      {
         gotoAndStop(7);
      }
      else
      {
         gotoAndStop(frame);
         wave += maxxspeed / 5;
         clip._y = Math.sin(wave) * 5;
         if(xspeed > 0)
         {
            _xscale = scale;
         }
         else
         {
            _xscale = - scale;
         }
         count++;
         if(count > waittime)
         {
            if(movedir == "right")
            {
               movedir = "left";
            }
            else
            {
               movedir = "right";
            }
            count = 0;
         }
         if(movedir == "right")
         {
            xspeed += 0.1;
         }
         else
         {
            xspeed -= 0.1;
         }
         xspeed = Math.max(- maxxspeed,xspeed);
         xspeed = Math.min(maxxspeed,xspeed);
         _X = _X + xspeed;
      }
   }
};
