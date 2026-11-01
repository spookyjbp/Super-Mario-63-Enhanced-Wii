onEnterFrame = function()
{
   if(_root.Course.Char.attackFrame == "none")
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
