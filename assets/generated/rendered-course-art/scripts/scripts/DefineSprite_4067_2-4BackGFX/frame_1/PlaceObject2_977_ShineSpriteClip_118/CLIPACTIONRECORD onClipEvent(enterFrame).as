onClipEvent(enterFrame){
   timecount++;
   if(_parent.bully.deadcount > 32 && _parent.bully.deadcount !== undefined && timecount > 12)
   {
      invis = false;
   }
}
