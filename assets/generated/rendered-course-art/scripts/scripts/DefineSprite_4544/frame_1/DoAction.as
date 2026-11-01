stop();
varplay = false;
if(Xscale == undefined)
{
   Xscale = _xscale;
}
depth = _root.Course.Platforms.getNextHighestDepth();
_root.Course.Platforms.attachMovie("BookGround","BookGround" + depth,depth,{_x:_X,_y:_Y,_yscale:_yscale,_xscale:Xscale,count:0,spincount:0,DirectionX:DirectionX,distanceX:distanceX,accelX:accelX,speedX:speedX,DirectionY:DirectionY,distanceY:distanceY,accelY:accelY,speedY:speedY,Xoffstart:Xoffstart,Yoffstart:Yoffstart});
offset = 0;
count = 0;
hit = false;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(_currentframe >= 123)
      {
         if(_root.Course.Char.air == false && this.box2.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _root.Course.Char.xspeed *= 0.7;
            _root.Course.Char.xspeed += Xscale / 50;
         }
      }
      if(_currentframe >= 2 && _currentframe <= 70)
      {
         if(_root.Course.Char.air == false && this.box2.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            _root.Course.Char.xspeed *= 0.7;
            _root.Course.Char.xspeed -= Xscale / 30;
         }
      }
      if(this.box.hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
      {
         offset = 160;
         count = 0;
         varplay = true;
      }
      else
      {
         count++;
         if(count > 10)
         {
            count = 0;
            offset = 0;
         }
      }
      tellTarget(_root.Course.Platforms["BookGround" + depth])
      {
         if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,_root.Course.Char._y * _root.coursescale / 100 + _root.Course._y,true))
         {
            this.varplay = true;
         }
         gotoAndStop(this._currentframe + this.offset);
      }
      if(this.varplay == true)
      {
         play();
      }
   }
   else
   {
      stop();
   }
};
