onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(Start == true)
      {
         _root.Course.BackGFX.fall0.Start = true;
         _rotation = _rotation + speed;
         _Y = _Y + fallspeed;
         fallspeed += 0.005;
         _X = _X - 0.4;
      }
      ydistance = _Y + _parent._y - _root.Course.Char._y;
      xdistance = _X + _parent._x - _root.Course.Char._x;
      distance = Math.sqrt(Math.pow(xdistance,2) + Math.pow(ydistance,2));
      rot = Math.atan2(ydistance,xdistance);
      xdiff = Math.cos(rot) * distance - Math.cos(rot + _root.angleToRadians(this.speed)) * distance;
      ydiff = Math.sin(rot) * distance - Math.sin(rot + _root.angleToRadians(this.speed)) * distance;
      if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + 3) * _root.coursescale / 100 + _root.Course._y,true) && _root.Course.Char.air == false)
      {
         Start = true;
         _root.Course.Char._x += xdiff - 0.4;
         _root.Course.Char._y += ydiff + fallspeed;
         if(landedon == false)
         {
            _root.Course.Char.xspeed -= xdiff - 0.4;
         }
         landedon = true;
      }
      else
      {
         if(landedon == true)
         {
            _root.Course.Char.xspeed += xdiff - 0.4;
            _root.Course.Char.yspeed += ydiff;
         }
         landedon = false;
      }
   }
}
