onEnterFrame = function()
{
   if(_root.BGquality == "HIGH")
   {
      light._rotation += 3;
      gotoAndStop(1);
   }
   else if(_root.BGquality == "MEDIUM")
   {
      light._rotation += 3;
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(3);
   }
};
