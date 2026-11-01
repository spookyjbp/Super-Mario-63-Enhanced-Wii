onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.xspeed = -4;
      _root.Course.Char.yspeed = -16;
   }
}
