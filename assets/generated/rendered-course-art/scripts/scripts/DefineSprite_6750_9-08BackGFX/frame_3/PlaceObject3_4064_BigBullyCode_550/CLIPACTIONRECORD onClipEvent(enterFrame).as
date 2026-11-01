onClipEvent(enterFrame){
   if(HP == 3)
   {
      scale = 2;
   }
   if(HP == 2)
   {
      scale = 2.5;
   }
   if(HP == 1)
   {
      scale = 3;
   }
   if(HP == 0)
   {
      scale -= 0.025;
      scale = Math.max(scale,0.5);
   }
   _yscale = scale * 50;
   if(_xscale > 0)
   {
      _xscale = scale * 50;
   }
   else
   {
      _xscale = (- scale) * 50;
   }
   _X = Math.max(_X,520);
   _X = Math.min(_X,1100);
}
