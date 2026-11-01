onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body) && _root.Invisible == true && _root.PowerTimer >= 0)
   {
      if(_root.PowerTimer >= 2)
      {
         _root.PowerTimer = 1;
      }
   }
}
