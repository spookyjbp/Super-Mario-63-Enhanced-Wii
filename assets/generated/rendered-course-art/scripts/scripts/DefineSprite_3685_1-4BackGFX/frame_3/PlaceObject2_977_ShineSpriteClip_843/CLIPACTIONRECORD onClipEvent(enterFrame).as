onClipEvent(enterFrame){
   timecount++;
   if(timecount > 12 && _root.kingBombdefeated == true)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      invis = false;
   }
}
