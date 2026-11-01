stop();
onEnterFrame = function()
{
   if(_root.lvl4GreenSwitch == true && _root.lvl4YellowSwitch == true)
   {
      gotoAndStop(2);
      onEnterFrame = null;
   }
   else
   {
      stop();
   }
};
