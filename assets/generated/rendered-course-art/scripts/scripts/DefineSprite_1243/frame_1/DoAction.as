stop();
_root.GetStarMusic1();
onEnterFrame = function()
{
   if(_root.Course.Char.air == false)
   {
      play();
   }
};
