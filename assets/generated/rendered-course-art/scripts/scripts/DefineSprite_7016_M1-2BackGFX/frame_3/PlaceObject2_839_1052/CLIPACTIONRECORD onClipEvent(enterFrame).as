onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "Dive";
      }
   }
}
