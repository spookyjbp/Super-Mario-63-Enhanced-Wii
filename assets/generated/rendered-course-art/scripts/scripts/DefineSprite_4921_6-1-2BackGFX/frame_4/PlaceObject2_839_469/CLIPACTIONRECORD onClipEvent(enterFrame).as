onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.Water == true)
      {
         _root.Course.Char.xspeed += 0.2;
      }
   }
}
