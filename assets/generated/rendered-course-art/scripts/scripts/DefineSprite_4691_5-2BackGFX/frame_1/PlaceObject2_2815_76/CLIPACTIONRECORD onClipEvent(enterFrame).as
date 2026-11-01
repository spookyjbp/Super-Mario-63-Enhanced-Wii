onClipEvent(enterFrame){
   angle = _root.angleToRadians(_root.Course.Platforms.Wheel._rotation + 90);
   rotation = angle + saverot;
   _rotation = _root.radiansToAngle(rotation);
   newx = Math.sin(rotation) * distance;
   newy = (- Math.cos(rotation)) * distance;
   _X = newx + 1350;
   _Y = newy + 30;
}
