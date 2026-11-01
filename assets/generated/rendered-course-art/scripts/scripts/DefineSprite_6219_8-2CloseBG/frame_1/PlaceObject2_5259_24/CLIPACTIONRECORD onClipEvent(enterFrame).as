onClipEvent(enterFrame){
   _rotation = _rotation + rotspeed;
   count += 1;
   if(count > 118)
   {
      rotspeed *= -1;
      count = 0;
   }
}
