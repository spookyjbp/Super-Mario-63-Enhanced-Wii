onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
      {
         _root.Course.Char.yspeed = _root.groundFriction(_root.Course.Char.yspeed,0.015,1.001);
         _root.Course.Char.yspeed -= 0.5;
         _root.Course.Char.fallcount = 0;
      }
   }
}
