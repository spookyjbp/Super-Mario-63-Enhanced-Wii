onClipEvent(enterFrame){
   timecount++;
   if(_root.L5VolStar == true && timecount > 12)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      invis = false;
   }
}
