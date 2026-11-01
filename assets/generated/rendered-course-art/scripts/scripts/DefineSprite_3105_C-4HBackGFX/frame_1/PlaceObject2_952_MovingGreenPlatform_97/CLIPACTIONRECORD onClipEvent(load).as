onClipEvent(load){
   DirectionX = "Right";
   DirectionY = "Down";
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
      _parent.td2._y = _Y - 20 - currentspeedY;
   };
}
