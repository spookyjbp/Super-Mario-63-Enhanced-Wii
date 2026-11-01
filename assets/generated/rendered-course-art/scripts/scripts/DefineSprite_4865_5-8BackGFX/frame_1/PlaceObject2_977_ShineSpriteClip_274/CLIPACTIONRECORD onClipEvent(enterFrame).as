onClipEvent(enterFrame){
   timecount++;
   if(_parent.bully.deadcount > 32 && _parent.bully.deadcount !== undefined && timecount > 12)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      invis = false;
   }
}
