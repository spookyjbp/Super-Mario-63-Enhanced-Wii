count = 0;
starcount = 1;
onEnterFrame = function()
{
   count++;
   if(count % 4)
   {
      starcount++;
      if(starcount <= 50)
      {
         this.attachMovie("Cinema2Scene3StarClip","star" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0});
      }
   }
   if(count < 2)
   {
      starcount++;
      if(starcount <= 50)
      {
         this.attachMovie("Cinema2Scene3StarClip","star" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0});
      }
      starcount++;
      if(starcount <= 50)
      {
         this.attachMovie("Cinema2Scene3StarClip","star" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0});
      }
      starcount++;
      if(starcount <= 50)
      {
         this.attachMovie("Cinema2Scene3StarClip","star" + this.getNextHighestDepth(),this.getNextHighestDepth(),{_x:0,_y:0});
      }
   }
};
