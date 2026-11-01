stop();
count = 0;
bubblecount = 0;
onEnterFrame = function()
{
   count++;
   if(count > rate)
   {
      if(th == false)
      {
         th = true;
         X = random(size / 2) + 1;
      }
      else
      {
         X = random(size / 2) + size / 2 - 1;
         th = false;
      }
      speedith = Math.random() * speed + 1;
      bubblecount++;
      this.attachMovie("LavaBubble","B" + bubblecount,this.getNextHighestDepth(),{_x:X,_y:0,_xscale:20,_yscale:20,speed:speedith,Yheight:Yheight});
      count = 0;
   }
};
