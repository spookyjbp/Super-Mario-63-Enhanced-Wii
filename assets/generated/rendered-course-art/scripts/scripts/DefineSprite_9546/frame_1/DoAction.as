onEnterFrame = function()
{
   if(_parent.gotstar == false)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
