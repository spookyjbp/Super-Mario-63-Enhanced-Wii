onEnterFrame = function()
{
   if(_parent.yspeed < 0.5)
   {
      gotoAndStop(1);
   }
   else
   {
      gotoAndStop(2);
   }
};
