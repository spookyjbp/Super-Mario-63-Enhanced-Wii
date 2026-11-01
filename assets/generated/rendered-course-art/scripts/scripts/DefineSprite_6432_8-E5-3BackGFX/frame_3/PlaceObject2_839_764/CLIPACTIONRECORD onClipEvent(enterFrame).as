onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.BackGFX.StartClip._y = 63;
      _root.Course.BackGFX.StartClip._x = -1337;
   }
}
