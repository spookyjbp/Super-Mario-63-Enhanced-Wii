if(_root.WingCap == false)
{
   nextFrame();
   onEnterFrame = function()
   {
      if(_root.WingCap == true)
      {
         gotoAndStop(1);
      }
   };
}
else
{
   onEnterFrame = function()
   {
      if(_root.WingCap == false)
      {
         nextFrame();
         onEnterFrame = function()
         {
            if(_root.WingCap == true)
            {
               gotoAndStop(1);
            }
         };
      }
   };
   stop();
}
