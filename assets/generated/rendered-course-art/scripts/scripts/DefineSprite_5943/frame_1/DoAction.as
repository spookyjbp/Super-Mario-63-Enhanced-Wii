onEnterFrame = function()
{
   if(_parent.nospin !== true)
   {
      _rotation = _rotation + 0.3;
   }
   _parent.rex = _X;
   _parent.rey = _Y;
};
