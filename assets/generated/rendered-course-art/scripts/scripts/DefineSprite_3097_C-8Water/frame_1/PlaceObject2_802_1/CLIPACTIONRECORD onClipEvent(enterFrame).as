onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(_root.C8Switch == true)
      {
         Y -= 0.5;
      }
      else
      {
         Y += 0.5;
      }
      Y = Math.max(Y,-150);
      Y = Math.min(Y,0);
      _Y = Y + 20;
   }
}
