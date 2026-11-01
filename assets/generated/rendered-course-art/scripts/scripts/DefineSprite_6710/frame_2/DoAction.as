count = 50;
if(_root.PauseGame == false && _root.playpause == false)
{
   play();
   count -= 1.5;
   count = Math.max(count,2);
   onEnterFrame = function()
   {
      _root.Course.Char.yspeed = 0;
      Y = _parent._y + _parent._parent._y;
      Y -= count;
      _root.Course.Char._y += (Y - _root.Course.Char._y) / 5;
      X = _parent._x + _parent._parent._x;
      _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
   };
}
else
{
   stop();
}
