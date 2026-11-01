onClipEvent(enterFrame){
   timecount++;
   if(_parent.bully1.deadcount > 32 && _parent.bully1.deadcount !== undefined && timecount > 12)
   {
      _root.Course.FrontGFX.gotoAndStop(1);
      invis = false;
   }
}
