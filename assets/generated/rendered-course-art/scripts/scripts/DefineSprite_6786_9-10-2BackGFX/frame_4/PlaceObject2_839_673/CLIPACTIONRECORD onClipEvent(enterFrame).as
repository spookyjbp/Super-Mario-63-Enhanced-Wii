onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.015,1.001);
         _root.Course.Char.xspeed += 0.9;
      }
   }
}
