onClipEvent(enterFrame){
   _Y = _Y + 3;
   _yscale = _yscale - 10;
   _yscale = Math.max(_yscale,0);
   _yscale = Math.min(_yscale,100);
}
