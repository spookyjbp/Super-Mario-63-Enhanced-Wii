onClipEvent(enterFrame){
   if(_X > 50)
   {
      _X = _X - 0.2;
      _Y = _Y + 0.2;
   }
   else if(_X > 30)
   {
      _X = _X - 0.1;
      _Y = _Y + 0.1;
   }
}
