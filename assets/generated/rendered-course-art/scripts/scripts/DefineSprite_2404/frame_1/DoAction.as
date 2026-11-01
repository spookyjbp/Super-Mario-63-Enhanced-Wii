stop();
_visible = false;
newx = _X;
newy = _Y;
oldx = _X;
oldy = _Y;
newerx = _X;
newery = _Y;
olderx = _X;
oldery = _Y;
onEnterFrame = function()
{
   _root.Course.Char.charClip.frame.followpath = true;
   if(_root.PauseGame == false)
   {
      if(isNaN(accelx))
      {
         accelx = 0;
      }
      if(isNaN(accely))
      {
         accely = 0;
      }
      if(isNaN(diffx))
      {
         diffx = 0;
      }
      if(isNaN(diffy))
      {
         diffy = 0;
      }
      _root.Course.Char._rotation = _rotation;
      _root.Course.Char.yspeed = this.diffy;
      _root.Course.Char.xspeed = this.diffx;
      distx = _parent._x + _parent._parent._x + _X;
      disty = _parent._y + _parent._parent._y + _Y;
      _root.Course.Char._x -= (_root.Course.Char._x - distx) / 10;
      _root.Course.Char._y -= (_root.Course.Char._y - disty) / 10;
      newx = _X;
      newy = _Y;
      diffy = newy - oldy;
      diffx = newx - oldx;
      oldx = newx;
      oldy = newy;
      newerx = diffx;
      newery = diffy;
      accely = newery - oldery;
      accelx = newerx - olderx;
      olderx = newerx;
      oldery = newery;
   }
};
