onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(Y - _root.Course.BackGFX.waterlevel > 3)
      {
         Y -= 2;
      }
      if(Y - _root.Course.BackGFX.waterlevel < -3)
      {
         Y += 2;
      }
      _Y = Y;
   }
}
