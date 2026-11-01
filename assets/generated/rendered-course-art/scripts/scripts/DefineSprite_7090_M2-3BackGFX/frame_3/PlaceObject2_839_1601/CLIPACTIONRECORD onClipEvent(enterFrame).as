onClipEvent(enterFrame){
   if(_root.Course.BackGFX.blok.count > 0)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
      {
         _root.Course.BackGFX.blok.count = Math.max(_root.Course.BackGFX.blok.count,850);
      }
   }
}
