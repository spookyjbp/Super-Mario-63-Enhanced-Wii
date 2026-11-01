count = 0;
_xscale = (random(2) - 0.5) * 200;
stop();
onEnterFrame = function()
{
   count++;
   if(count > num)
   {
      play();
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
      };
   }
};
