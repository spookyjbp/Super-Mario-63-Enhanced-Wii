onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.Course.Char.yspeed -= 0.4;
         _root.Course.Char.fallcount = 0;
      }
   }
}
