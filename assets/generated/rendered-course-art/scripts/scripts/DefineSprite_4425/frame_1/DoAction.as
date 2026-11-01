onEnterFrame = function()
{
   if(_root._quality == "HIGH" || _root._quality == "BEST")
   {
      gotoAndStop(1);
   }
   else
   {
      gotoAndStop(2);
   }
};
