count = 12;
if(_parent._parent.bounce == true)
{
   if(_root.Course.Char.attack == false)
   {
      if(_root.KeyUP())
      {
         _root.Course.Char.landtime = 1;
      }
      else
      {
         _root.Course.Char.landtime = 0;
      }
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "EJump";
   }
   if(_root.PauseGame == false && _root.playpause == false)
   {
      play();
      count -= 1.5;
      count = Math.max(count,3);
      onEnterFrame = function()
      {
         _root.Course.Char.yspeed = 0;
         xdivide = Math.abs(_root.Course.Char.xspeed / 2) + 5;
         Y = _parent._parent._y + _parent._parent._parent._y;
         Y -= count;
         if(Math.abs(Y - _root.Course.Char._y) < 15)
         {
            _root.Course.Char._y += (Y - _root.Course.Char._y) / 5;
         }
         X = _parent._parent._x + _parent._parent._parent._x;
         if(Math.abs(X - _root.Course.Char._x) < 15)
         {
            _root.Course.Char._x += (X - _root.Course.Char._x) / xdivide;
         }
      };
   }
   else
   {
      stop();
   }
}
