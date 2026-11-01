stop();
count = 0;
onEnterFrame = function()
{
   count++;
   if(count > 8)
   {
      play();
   }
};
