onEnterFrame = function()
{
   if(_parent._parent.yspeed > 0)
   {
      gotoAndStop(2);
   }
   else
   {
      gotoAndStop(1);
   }
};
