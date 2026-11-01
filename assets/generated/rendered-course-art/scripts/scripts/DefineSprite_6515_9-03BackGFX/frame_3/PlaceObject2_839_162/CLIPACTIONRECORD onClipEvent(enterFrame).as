onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.BackGFX.Switched = "gone";
   }
}
