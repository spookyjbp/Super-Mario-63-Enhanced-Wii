onClipEvent(enterFrame){
   if(_root.RedCoins >= 8)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      invis = false;
   }
}
