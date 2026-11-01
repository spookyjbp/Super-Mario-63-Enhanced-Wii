onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(started == true)
      {
         _rotation = _rotation + speed;
         _Y = _Y + fallspeed;
      }
   }
}
