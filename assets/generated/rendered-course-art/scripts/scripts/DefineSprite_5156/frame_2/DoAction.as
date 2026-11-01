stop();
newx = _X;
newy = _Y;
oldx = _X;
oldy = _Y;
newerx = _X;
newery = _Y;
olderx = _X;
oldery = _Y;
if(_parent.exists == undefined && isNaN(_root.Course.Platforms["RainbowCarpetPL" + _parent.capetdepth].exists))
{
   _parent.capetdepth = _root.Course.Platforms.getNextHighestDepth();
   _root.Course.Platforms.attachMovie("CustomMovingPL","CustomMovingPL" + _parent.capetdepth,_parent.capetdepth,{_x:_X + _parent._x,_y:_Y + _parent._y,scale:scale,exists:true});
   _parent.exists = true;
}
onEnterFrame = function()
{
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
      tellTarget(_root.Course.Platforms["CustomMovingPL" + _parent.capetdepth])
      {
         if(hitTest((_root.Course.Char._x + this.diffx + this.accelx) * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.diffy + this.accely + 3) * _root.coursescale / 100 + _root.Course._y,true))
         {
            _root.Course.Char._x += this.diffx + this.accelx;
            _root.Course.Char._y += this.diffy;
            _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.3,1.1);
            if(hitTest((_root.Course.Char._x + this.diffx + this.accelx) * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.diffy + this.accely - 3) * _root.coursescale / 100 + _root.Course._y,true))
            {
               _root.Course.Char._y--;
            }
            if(landedon == false)
            {
               _root.Course.Char.xspeed -= this.diffx;
            }
            landedon = true;
            _parent.landedon = true;
         }
         else
         {
            if(landedon == true)
            {
               _root.Course.Char.xspeed += this.diffx;
               _root.Course.Char.yspeed += this.diffy / 2;
            }
            landedon = false;
            _parent.landedon = false;
         }
         _Y = this._y + this._parent._y;
         _X = this._x + this._parent._x;
      }
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
