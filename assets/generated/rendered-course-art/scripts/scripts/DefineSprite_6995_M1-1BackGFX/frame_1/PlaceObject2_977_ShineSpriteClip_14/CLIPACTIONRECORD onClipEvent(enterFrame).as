onClipEvent(enterFrame){
   timecount++;
   if(_root.RedCoins >= 8 && timecount > 12)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      invis = false;
   }
}
