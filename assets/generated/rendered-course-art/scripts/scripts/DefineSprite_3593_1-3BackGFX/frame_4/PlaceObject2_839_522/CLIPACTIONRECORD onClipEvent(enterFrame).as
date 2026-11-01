onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
      {
         _root.Course.Char.xspeed = _root.groundFriction(_root.Course.Char.xspeed,0.03,1.03);
         _root.Course.Char.xspeed -= 0.7;
         _root.Course.Char.yspeed = _root.groundFriction(_root.Course.Char.yspeed,0.03,1.01);
         _root.Course.Char.yspeed -= 1.3;
      }
   }
}
