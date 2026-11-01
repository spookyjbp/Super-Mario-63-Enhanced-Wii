count = 7;
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
   if(_root.PauseGame == false)
   {
      play();
      if(_parent._parent.scale > 1)
      {
         _root.Course.Char._y += 3;
      }
      count -= 1.5 * _parent._parent.scale;
      count = Math.max(count,2);
      onEnterFrame = function()
      {
         _root.Course.Char.yspeed = 0;
         xdivide = Math.abs(_root.Course.Char.xspeed / 2) + 5;
         Y = _parent._parent._y + _parent._parent._parent._y;
         Y -= count * _parent._parent.scale;
         if(Math.abs(Y - _root.Course.Char._y) < 15)
         {
            _root.Course.Char._y += (Y - _root.Course.Char._y) / (5 / _parent._parent.scale);
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
