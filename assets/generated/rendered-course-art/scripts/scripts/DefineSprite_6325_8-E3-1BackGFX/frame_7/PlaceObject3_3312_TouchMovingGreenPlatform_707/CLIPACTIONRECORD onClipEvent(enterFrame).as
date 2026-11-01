onClipEvent(enterFrame){
   if(_alpha < 100)
   {
      this._alpha += 1;
   }
   else
   {
      _alpha = 100;
   }
   OEF();
}
