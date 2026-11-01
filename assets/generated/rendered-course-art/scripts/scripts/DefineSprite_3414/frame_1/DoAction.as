onEnterFrame = function()
{
   if(_root.Course.BackGFX.inroom == true)
   {
      _alpha = Math.max(_alpha - 3,0);
   }
   else
   {
      _alpha = Math.min(_alpha + 3,100);
   }
};
