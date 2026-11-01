onClipEvent(enterFrame){
   _rotation = _rotation + rotspeed;
   count += 1;
   if(count > 80)
   {
      rotspeed *= -1;
      count = 0;
   }
}
