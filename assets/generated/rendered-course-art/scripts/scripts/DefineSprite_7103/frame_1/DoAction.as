onEnterFrame = function()
{
   if(_root.Course.BackGFX.SwitchPL == true)
   {
      gotoAndStop(1);
   }
   else
   {
      gotoAndStop(2);
   }
};
stop();
