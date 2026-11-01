count = 0;
onEnterFrame = function()
{
   _X = _X + 2;
   count += 2;
   if(count >= 32)
   {
      count -= 32;
      _X = _X - 32;
   }
};
