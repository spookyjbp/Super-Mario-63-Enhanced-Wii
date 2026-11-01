stop();
if(speed == undefined)
{
   speed = 3;
}
onEnterFrame = function()
{
   i = 0;
   while(i < speed)
   {
      nextFrame();
      i++;
   }
};
