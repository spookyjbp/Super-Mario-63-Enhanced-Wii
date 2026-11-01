_root.playpause = true;
onEnterFrame = function()
{
   X = _X + _parent._x;
   _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
};
