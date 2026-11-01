onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.xspeed = 1;
      _root.Course.Char.yspeed = -20;
   }
}
