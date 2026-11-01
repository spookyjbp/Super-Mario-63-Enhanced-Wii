onClipEvent(enterFrame){
   _rotation = _rotation + rotspeed;
   count += 1;
   if(count > 260)
   {
      rotspeed *= -1;
      count = 0;
   }
   dx = _X - _root.Course.Char._x + _parent._x;
   dy = _Y - _root.Course.Char._y + _parent._y;
   distance = Math.sqrt(dx * dx + dy * dy);
   newx = Math.sin(_root.angleToRadians(_rotation)) * distance;
   newy = Math.cos(_root.angleToRadians(_rotation)) * distance;
   diffy = newy - oldy;
   diffx = newx - oldx;
   oldx = newx;
   oldy = newy;
   Y = Math.cos(_root.angleToRadians(_rotation)) * distance;
   X = Math.sin(_root.angleToRadians(_rotation)) * distance;
   if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true))
   {
      _root.Course.Char._x += this.diffx;
      _root.Course.Char._y += this.diffY;
      if(landedon == false)
      {
         _root.Course.Char.xspeed -= this.diffx;
      }
      landedon = true;
   }
   else
   {
      if(landedon == true)
      {
         _root.Course.Char.xspeed += this.diffx;
      }
      landedon = false;
   }
}
