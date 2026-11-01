onClipEvent(enterFrame){
   if(_root.Course.Char._x - _root.Course.BackGFX._x < -2400 && g == false)
   {
      play();
      g = true;
   }
}
