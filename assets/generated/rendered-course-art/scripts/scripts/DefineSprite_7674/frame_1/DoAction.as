Start = function()
{
   clip.removeMovieClip();
   clip2.removeMovieClip();
   RES();
   onEnterFrame = function()
   {
      OEF();
   };
};
RES = function()
{
   if(scale == undefined)
   {
      scale = 2;
   }
   if(angle == undefined)
   {
      angle = 0;
   }
   if(distance == undefined)
   {
      distance = 50;
   }
   if(speed == undefined)
   {
      speed = 5;
   }
   if(dotdistance == undefined)
   {
      dotdistance = 20;
   }
   if(armscale == undefined)
   {
      armscale = 0;
   }
   if(num == undefined)
   {
      num = 2;
   }
   this.createEmptyMovieClip("clip",this.getNextHighestDepth());
   if(dotdistance < 360)
   {
      i = 0;
      while(i < 360)
      {
         clip.attachMovie("Path dot","dot" + clip.getNextHighestDepth(),clip.getNextHighestDepth(),{_y:Math.cos(_root.angleToRadians(i)) * distance,_x:(- Math.sin(_root.angleToRadians(i))) * distance});
         i += dotdistance;
      }
   }
   this.createEmptyMovieClip("clip2",this.getNextHighestDepth());
   i = 0;
   while(i < num)
   {
      trace(scale);
      clip2.attachMovie("GreenPLClip","pl" + i,clip2.getNextHighestDepth());
      clip2["pl" + i].gotoAndStop(scale);
      i++;
   }
};
OEF = function()
{
   angle += speed;
   i = 0;
   while(i < num)
   {
      clip2["pl" + i]._x = (- Math.sin(_root.angleToRadians(angle + 360 / num * i))) * distance;
      clip2["pl" + i]._y = Math.cos(_root.angleToRadians(angle + 360 / num * i)) * distance;
      i++;
   }
};
this.createEmptyMovieClip("clip2",this.getNextHighestDepth());
clip2.attachMovie("GreenPLClip","pl0",clip2.getNextHighestDepth());
clip2.pl0.gotoAndStop(2);
nextFrame();
