stop();
count = 0;
num = random(100) + 10;
onEnterFrame = function()
{
   count++;
   if(count > num)
   {
      nextFrame();
   }
};
