onClipEvent(enterFrame){
   timecount++;
   if(_root.SilverStars >= 5 && timecount > 12)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      invis = false;
   }
}
