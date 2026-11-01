onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         if(_root.Course.Char.xspeed < -3)
         {
            _root.Course.Char.xspeed *= 0.96;
            _root.Course.Char.xspeed += 0.1;
         }
      }
   }
}
