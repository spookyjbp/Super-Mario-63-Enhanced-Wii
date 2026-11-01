onClipEvent(enterFrame){
   _rotation = _rotation + rotspeed;
   count += 1;
   if(count > 260)
   {
      rotspeed *= -1;
      count = 0;
   }
}
