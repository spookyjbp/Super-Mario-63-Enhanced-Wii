onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
      {
         if(_root.Course.Char.yspeed > -2)
         {
            _root.Course.Char.yspeed -= 4;
            _root.Course.Char.xspeed -= 0.1;
            _root.Course.Char.fallcount = 0;
         }
      }
   }
}
