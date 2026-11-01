stop();
_alpha = 0;
onEnterFrame = function()
{
   if(_alpha <= 0)
   {
      gotoAndStop(1);
   }
   else
   {
      gotoAndStop(2);
   }
};
