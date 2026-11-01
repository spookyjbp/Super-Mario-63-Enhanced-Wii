onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char.xspeed = Math.random() * 2 - 6;
      _root.Course.Char.yspeed = -20;
   }
}
