onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.BackGFX.StartClip._y = _root.Course.BackGFX.restart3._y;
      _root.Course.BackGFX.StartClip._x = _root.Course.BackGFX.restart3._x;
   }
}
