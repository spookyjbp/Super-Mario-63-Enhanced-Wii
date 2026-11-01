onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.Course.Char._y -= 400;
      _root.MarioHit(0,6,-7,-2,10,"Mario-Squish",true);
   }
}
