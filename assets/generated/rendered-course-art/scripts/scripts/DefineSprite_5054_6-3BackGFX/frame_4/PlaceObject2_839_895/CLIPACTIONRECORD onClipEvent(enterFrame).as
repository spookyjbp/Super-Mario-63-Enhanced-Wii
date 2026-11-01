onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.Power += 3;
         _root.Course.Char.fallcount = 0;
         _root.Course.Char.yspeed -= 0.5;
      }
   }
}
