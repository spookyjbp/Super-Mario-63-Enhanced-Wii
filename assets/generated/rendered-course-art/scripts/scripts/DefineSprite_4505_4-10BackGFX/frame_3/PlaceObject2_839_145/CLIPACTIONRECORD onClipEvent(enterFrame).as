onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body) && _root.Course.FrontGFX.Boo.HP > 2)
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.TextHint = mess;
      }
   }
   else if(mess == _root.TextHint)
   {
      _root.TextHint = "";
   }
}
