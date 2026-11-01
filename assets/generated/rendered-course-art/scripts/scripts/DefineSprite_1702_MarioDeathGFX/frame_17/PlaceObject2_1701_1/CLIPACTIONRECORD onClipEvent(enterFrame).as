onClipEvent(enterFrame){
   if(yspeed > 0)
   {
      _rotation = _rotation + 15;
   }
   _Y = _Y + (yspeed += 0.7);
}
