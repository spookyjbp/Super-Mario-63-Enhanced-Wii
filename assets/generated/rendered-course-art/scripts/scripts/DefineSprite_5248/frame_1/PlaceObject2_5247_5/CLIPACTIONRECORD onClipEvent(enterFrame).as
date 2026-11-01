onClipEvent(enterFrame){
   _rotation = _rotation + rotspeed;
   count += 1;
   if(count > 250)
   {
      rotspeed *= -1;
      count = 0;
   }
}
