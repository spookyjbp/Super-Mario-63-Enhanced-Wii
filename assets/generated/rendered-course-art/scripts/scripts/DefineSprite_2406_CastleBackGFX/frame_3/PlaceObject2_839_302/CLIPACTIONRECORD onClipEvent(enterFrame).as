onClipEvent(enterFrame){
   if(_root.TotalStars <= 0 && _root.Star[39] == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
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
}
