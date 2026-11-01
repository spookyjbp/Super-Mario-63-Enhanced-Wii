count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
   }
   else
   {
      stop();
   }
   count++;
   if(count % 32 == 0)
   {
      if(random(2) == 0)
      {
         GoombaSummon(0,-600,Math.random() * 8 - 4,7);
      }
      else
      {
         BobombSummon(0,-600,Math.random() * 8 - 4,7);
      }
   }
};
