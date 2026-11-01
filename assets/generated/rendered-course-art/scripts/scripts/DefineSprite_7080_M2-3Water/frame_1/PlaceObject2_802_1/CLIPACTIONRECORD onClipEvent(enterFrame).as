onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(Y - _root["M2-3WaterLevel"] > 5)
      {
         Y -= 5;
      }
      if(Y - _root["M2-3WaterLevel"] < -5)
      {
         Y += 5;
      }
      _Y = Y;
   }
}
