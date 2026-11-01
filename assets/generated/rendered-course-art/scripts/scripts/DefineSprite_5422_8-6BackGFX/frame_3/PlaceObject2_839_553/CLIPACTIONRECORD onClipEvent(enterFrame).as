onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
      {
         _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.01,1.01);
         _root.Course.Char.xspeed -= 0.3;
      }
   }
}
