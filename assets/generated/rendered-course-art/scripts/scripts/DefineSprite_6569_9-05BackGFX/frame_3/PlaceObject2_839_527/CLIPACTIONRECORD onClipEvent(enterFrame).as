onClipEvent(enterFrame){
   if(this.hitTest(_root.Course.Char.charClip.body))
   {
      if(_root.PauseGame == false && _root.playpause == false)
      {
         _root.Course.Char.yspeed -= 0.9;
         _root.Course.Char.xspeed -= 0.1;
         _root.Course.Char.fallcount = 0;
         if(_root.Course.Char.yspeed < 0)
         {
            _root.Course.Char.yspeed -= 0.4;
         }
      }
   }
}
