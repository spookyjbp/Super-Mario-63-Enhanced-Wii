onClipEvent(enterFrame){
   if(_root.Course.Char._x - _root.Course.BackGFX._x < -1000 && g !== true)
   {
      play();
      g = true;
   }
}
