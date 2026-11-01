stop();
count = 0;
onEnterFrame = function()
{
   count++;
   if(count > 64)
   {
      play();
      count = undefined;
      onEnterFrame = undefined;
   }
};
