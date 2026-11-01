onClipEvent(enterFrame){
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(this.hitTest(_root.Course.Char.charClip.body))
      {
         _root.changestep = 3;
         _root.StepSound = "step-wetwood";
      }
   }
}
