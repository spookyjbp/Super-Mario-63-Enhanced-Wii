stop();
if(DirectionX == undefined)
{
   DirectionX = "Right";
}
if(DirectionY == undefined)
{
   DirectionY = "Up";
}
if(distanceX == undefined)
{
   distanceX = 25;
}
if(distanceY == undefined)
{
   distanceY = 0;
}
if(accelX == undefined)
{
   accelX = 1;
}
if(accelY == undefined)
{
   accelY = 0;
}
if(speedX == undefined)
{
   speedX = 5;
}
if(speedY == undefined)
{
   speedY = 0;
}
if(Xoffstart == undefined)
{
   Xoffstart = 0;
}
if(Yoffstart == undefined)
{
   Yoffstart = 0;
}
if(scale == undefined)
{
   scale = 2;
}
if(DirectionY == "Up")
{
   _Y = _Y - speedY;
}
else
{
   _Y = _Y + speedY;
}
if(DirectionY == "Right")
{
   _X = _X - speedX;
}
else
{
   _X = _X + speedX;
}
if(pauses == undefined)
{
   pauses = true;
}
pl.gotoAndStop(scale);
depth = _root.Course.BPlatforms.getNextHighestDepth();
_root.Course.BPlatforms.attachMovie("GreenPLcode","GreenPL" + depth,depth,{_x:_X,_y:_Y,scale:scale});
tellTarget(_root.Course.BPlatforms["GreenPL" + depth])
{
   currentspeedX = 0;
   distancecountX = speedX + Xoffstart;
   currentspeedY = 0;
   distancecountY = speedY + Yoffstart;
   landedon = false;
   lessthan = false;
   morethan = false;
}
currentspeedX = 0;
distancecountX = speedX + Xoffstart;
currentspeedY = 0;
distancecountY = speedY + Yoffstart;
onEnterFrame = function()
{
   if(_root.PauseGame == false && (_root.playpause == false || pauses == false))
   {
      if(currentspeedY < 0)
      {
         pl._y = - currentspeedY;
      }
      else
      {
         pl._y = 0;
      }
      distancecountX++;
      if(distancecountX >= distanceX)
      {
         distancecountX = 0;
         if(DirectionX == "Right")
         {
            DirectionX = "Left";
         }
         else
         {
            DirectionX = "Right";
         }
      }
      if(DirectionX == "Right")
      {
         currentspeedX += accelX;
      }
      else
      {
         currentspeedX -= accelX;
      }
      distancecountY++;
      if(distancecountY >= distanceY)
      {
         distancecountY = 0;
         if(DirectionY == "Up")
         {
            DirectionY = "Down";
         }
         else
         {
            DirectionY = "Up";
         }
      }
      if(DirectionY == "Up")
      {
         currentspeedY -= accelY;
      }
      else
      {
         currentspeedY += accelY;
      }
      currentspeedX = Math.max(- speedX,currentspeedX);
      currentspeedX = Math.min(speedX,currentspeedX);
      currentspeedY = Math.max(- speedY,currentspeedY);
      currentspeedY = Math.min(speedY,currentspeedY);
      _X = _X + currentspeedX;
      _Y = _Y + currentspeedY;
      if(Math.abs(_root.currentCamX - (_X + _parent._x)) < 600 && Math.abs(_root.currentCamY - (_Y + _parent._y)) < 600)
      {
         tellTarget(_root.Course.BPlatforms["GreenPL" + depth])
         {
            if(hitTest(_root.Course.Char._x * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y + this.currentspeedY + 3) * _root.coursescale / 100 + _root.Course._y,false) && _root.Course.Char.yspeed > -1)
            {
               _root.Course.Char._x += this.currentspeedX;
               _root.Course.Char._y += this.currentspeedY;
               _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.3,1.1);
               if(landedon == false)
               {
                  _root.Course.Char.xspeed -= this.currentspeedX / 2;
               }
               landedon = true;
            }
            else
            {
               if(landedon == true)
               {
                  _root.Course.Char.xspeed += this.currentspeedX;
                  _root.Course.Char.yspeed += this.currentspeedY;
               }
               landedon = false;
            }
            _Y = this._y;
            _X = this._x;
         }
      }
   }
   if(_root.PauseGame == false)
   {
      enterframe();
   }
};
