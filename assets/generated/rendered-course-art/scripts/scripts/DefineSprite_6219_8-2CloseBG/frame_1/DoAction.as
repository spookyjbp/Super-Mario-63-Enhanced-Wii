onEnterFrame = function()
{
   if(_root.BGquality == "HIGH" || _root.BGquality == "MEDIUM")
   {
      gotoAndStop(1);
   }
   else
   {
      gotoAndStop(2);
   }
};
