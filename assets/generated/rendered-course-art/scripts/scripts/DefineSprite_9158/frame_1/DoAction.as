stop();
onEnterFrame = function()
{
   if(_root.OrangeBlockPLCount <= 0)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
