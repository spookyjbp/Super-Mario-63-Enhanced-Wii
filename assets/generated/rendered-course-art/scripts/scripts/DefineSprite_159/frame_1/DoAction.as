onEnterFrame = function()
{
   if(_root.BGquality !== "LOW")
   {
      this.gotoAndStop(1);
   }
   else
   {
      this.gotoAndStop(2);
   }
};
