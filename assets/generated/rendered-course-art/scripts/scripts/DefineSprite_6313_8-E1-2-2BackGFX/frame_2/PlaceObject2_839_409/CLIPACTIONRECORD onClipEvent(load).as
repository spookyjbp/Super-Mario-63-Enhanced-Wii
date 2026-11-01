onClipEvent(load){
   _visible = false;
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      _root.AreaTextClipF("Mushroom Kingdom (Tutorial)",45);
   }
}
