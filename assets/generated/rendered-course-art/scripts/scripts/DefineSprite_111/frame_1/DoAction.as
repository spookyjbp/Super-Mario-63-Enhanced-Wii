frame = _root.CharHP + 1;
onEnterFrame = function()
{
   if(_currentframe - 1 < _root.CharHP)
   {
      frame += 0.3;
   }
   if(_currentframe - 1 > _root.CharHP)
   {
      frame -= 0.3;
   }
   gotoAndStop(Math.round(frame));
};
