onClipEvent(enterFrame){
   if(_parent.side == "Right")
   {
      _rotation = _rotation + 4.5;
   }
   else if(_parent.side == "Left")
   {
      _rotation = _rotation - 4.5;
   }
   else
   {
      _rotation = _rotation + 2.25;
   }
}
