onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      count++;
      _Y = _Y + 0.5;
      _X = _X + Math.cos(count) * 0.5;
   }
}
