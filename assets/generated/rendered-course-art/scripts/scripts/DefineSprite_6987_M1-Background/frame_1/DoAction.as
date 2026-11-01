onEnterFrame = function()
{
   if(_root.BGquality == "HIGH")
   {
      gotoAndStop(1);
   }
   else if(_root.BGquality == "MEDIUM")
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(3);
   }
};
