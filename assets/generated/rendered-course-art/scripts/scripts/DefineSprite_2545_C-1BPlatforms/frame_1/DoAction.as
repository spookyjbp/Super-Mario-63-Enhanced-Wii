stop();
onEnterFrame = function()
{
   if(_root.Course.Char.air == false)
   {
      nextFrame();
      onEnterFrame = null;
   }
};
