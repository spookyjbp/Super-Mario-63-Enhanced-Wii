onClipEvent(enterFrame){
   if(_root.Course.Char._x - _root.Course.BackGFX._x < -750 && a == undefined)
   {
      a = true;
      play();
   }
}
