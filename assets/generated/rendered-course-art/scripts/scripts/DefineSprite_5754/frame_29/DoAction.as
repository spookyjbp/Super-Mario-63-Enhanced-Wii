stop();
b._alpha = 100;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      b.s._rotation += 3;
      b._xscale *= 1.05;
      b._yscale = b._xscale;
      if(b._xscale >= 80)
      {
         gotoAndPlay(1);
      }
   }
};
