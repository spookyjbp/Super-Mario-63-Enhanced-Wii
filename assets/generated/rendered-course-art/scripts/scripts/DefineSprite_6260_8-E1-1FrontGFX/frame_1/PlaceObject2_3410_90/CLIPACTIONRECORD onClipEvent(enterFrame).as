onClipEvent(enterFrame){
   if(_root.Course.Platforms._currentframe !== 1)
   {
      _alpha = _alpha - 10;
      if(_alpha <= 0)
      {
         this._y = 2000;
         this.swapDepths(0);
         this.removeMovieClip();
      }
   }
}
