onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(Y - _root["M2-1WaterLevel"] > 3)
      {
         Y -= 2;
      }
      if(Y - _root["M2-1WaterLevel"] < -3)
      {
         Y += 2;
      }
      _Y = Y + 420;
   }
}
