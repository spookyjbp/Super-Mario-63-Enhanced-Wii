stop();
onEnterFrame = function()
{
   if(_root.Course.Char.air == false)
   {
      gotoAndStop(2);
      onEnterFrame = null;
   }
};
