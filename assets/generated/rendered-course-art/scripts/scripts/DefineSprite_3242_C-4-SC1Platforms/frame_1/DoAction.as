count = 0;
stop();
onEnterFrame = function()
{
   if(_root.Course.Char.attackFrame == "Star" && count++ > 3)
   {
      nextFrame();
   }
};
