onEnterFrame = function()
{
   distance = Math.abs(_X + _parent._x - _root.Course.Char._x);
   distance *= 2;
   distance -= 70;
   distance = Math.max(distance,0);
   distance = Math.min(distance,100);
   peach._alpha = distance;
};
