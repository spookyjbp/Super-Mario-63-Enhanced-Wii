frame = _root.WaterHP + 1;
onEnterFrame = function()
{
   if(_currentframe - 1 < _root.WaterHP)
   {
      frame += 0.3;
   }
   if(_currentframe - 1 > _root.WaterHP)
   {
      frame -= 0.3;
   }
   gotoAndStop(Math.round(frame));
};
