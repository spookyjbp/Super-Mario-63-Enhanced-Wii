onClipEvent(load){
   DirectionX = "Right";
   DirectionY = "Up";
   distanceX = 0;
   distanceY = 116.66666666666667;
   accelX = 0;
   accelY = 0.2;
   speedX = 0;
   speedY = 3;
   scale = 3;
   pauses = false;
   enterframe = function()
   {
      _parent.td1._y = _Y - 20 - currentspeedY;
   };
}
