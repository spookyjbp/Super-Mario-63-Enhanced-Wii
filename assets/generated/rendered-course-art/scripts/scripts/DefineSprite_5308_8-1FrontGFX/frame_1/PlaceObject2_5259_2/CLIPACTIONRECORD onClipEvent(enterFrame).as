onClipEvent(enterFrame){
   _rotation = _rotation + rotspeed;
   count += 1;
   if(count > 160)
   {
      rotspeed *= -1;
      count = 0;
   }
}
