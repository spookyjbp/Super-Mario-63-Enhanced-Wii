onEnterFrame = function()
{
   if(_parent.yspeed > 0)
   {
      gotoAndStop(1);
   }
   else if(_parent.yspeed > 1)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(3);
   }
};
