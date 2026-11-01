onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.yspeed = -14;
      _root.Course.Char.xspeed = -2;
   }
}
