onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.feet))
      {
         _root.Course.Char.hurtfallcount = 0;
      }
   }
}
