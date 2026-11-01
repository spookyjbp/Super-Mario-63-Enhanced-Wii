stop();
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count++;
      if(count > 300)
      {
         gotoAndStop(1);
         onEnterFrame = null;
      }
   }
};
