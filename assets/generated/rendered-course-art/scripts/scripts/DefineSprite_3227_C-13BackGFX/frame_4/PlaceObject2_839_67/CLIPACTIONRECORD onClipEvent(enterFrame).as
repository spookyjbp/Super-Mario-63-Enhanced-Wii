onClipEvent(enterFrame){
   count++;
   if(count < 3 && this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.charClip.frame.changemusic = false;
      _parent.inroom = true;
   }
}
