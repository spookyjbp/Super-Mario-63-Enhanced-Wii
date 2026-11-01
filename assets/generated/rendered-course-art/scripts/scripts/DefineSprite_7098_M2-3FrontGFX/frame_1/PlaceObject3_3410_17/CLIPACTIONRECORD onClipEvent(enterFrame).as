onClipEvent(enterFrame){
   if(_root["M2-3WaterLevel"] > 1000)
   {
      _alpha = _alpha - 10;
      if(_alpha <= 0)
      {
         this.swapDepths(0);
         this.removeMovieClip();
      }
   }
}
