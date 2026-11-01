onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.feet) && _root.Course.Char.air == false)
      {
         landon = true;
         _root.Course.Char._x -= 1;
      }
      else
      {
         if(landon == true)
         {
            _root.Course.Char.xspeed -= 1;
         }
         landon = false;
      }
   }
}
