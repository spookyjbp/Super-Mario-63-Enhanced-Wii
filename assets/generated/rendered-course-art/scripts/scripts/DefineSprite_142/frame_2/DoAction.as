cl._alpha = 0;
cl._yscale = random(20) + 15;
onEnterFrame = function()
{
   if(_root.WaterPercent >= 10)
   {
      cl._alpha += 15;
      cl._y--;
      cl._yscale += random(5);
      cl._xscale = cl._yscale;
      if(cl._alpha >= 100)
      {
         onEnterFrame = function()
         {
            cl._y--;
            if(cl._y < - _root.WaterPercent * 1.1)
            {
               cl._alpha -= 15;
            }
            if(cl._alpha <= 0)
            {
               prevFrame();
            }
         };
      }
   }
};
