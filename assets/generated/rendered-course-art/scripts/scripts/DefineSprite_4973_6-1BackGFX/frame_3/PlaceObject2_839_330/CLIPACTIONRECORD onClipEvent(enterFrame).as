onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body) && _root.Course.Char.air == true)
      {
         _root.Course.Char.yspeed = _root.groundFriction(_root.Course.Char.yspeed,0.035,1.005);
         _root.Course.Char.yspeed -= 3;
         _root.Course.Char.xspeed += 0.05;
      }
   }
}
