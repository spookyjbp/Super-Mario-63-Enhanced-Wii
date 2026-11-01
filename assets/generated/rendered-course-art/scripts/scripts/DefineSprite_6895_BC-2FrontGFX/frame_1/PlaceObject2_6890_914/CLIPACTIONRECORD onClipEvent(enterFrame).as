onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      _rotation = _rotation + speed;
      _Y = _Y + fallspeed;
      if(_Y < -600)
      {
         _Y = _Y + 1400;
      }
   }
}
