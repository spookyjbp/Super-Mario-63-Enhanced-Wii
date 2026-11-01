Y = this._y;
sin = 0;
sin2 = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      b.s._rotation += 0.3;
      b._xscale = 80 + Math.sin(sin2) * 5;
      b._yscale = b._xscale;
      sin += 0.01;
      sin2 += 0.015;
      this._y = Y + Math.sin(sin) * 8;
   }
};
Explode = function()
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         b._xscale *= 1.1;
         b.s._rotation++;
         b._alpha -= 10;
         b._yscale = b._xscale;
         nextFrame();
         if(b._alpha <= 0)
         {
            b._alpha = 0;
            b._xscale = 50;
            b._yscale = b._xscale;
            onEnterFrame = function()
            {
               if(_root.PauseGame == false)
               {
                  nextFrame();
               }
            };
         }
      }
   };
};
