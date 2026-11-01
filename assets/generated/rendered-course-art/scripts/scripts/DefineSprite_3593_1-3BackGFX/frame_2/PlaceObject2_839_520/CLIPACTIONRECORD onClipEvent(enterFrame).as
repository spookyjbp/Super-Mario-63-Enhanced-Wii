onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.xspeed = Math.random() * 4 - 2;
      _root.Course.Char.yspeed = -14;
   }
}
