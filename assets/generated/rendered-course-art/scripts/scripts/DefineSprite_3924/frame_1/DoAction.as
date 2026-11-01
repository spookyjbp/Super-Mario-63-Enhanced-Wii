onEnterFrame = function()
{
   if(_root.BGquality == "HIGH")
   {
      gotoAndStop(2);
   }
   else if(_root.BGquality == "MEDIUM")
   {
      gotoAndStop(3);
   }
   else
   {
      gotoAndStop(4);
   }
};
