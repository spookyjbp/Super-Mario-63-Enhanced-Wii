onClipEvent(enterFrame){
   count++;
   if(keycount >= 2 && count > 12)
   {
      _root["coindata" + _root.playingcourse][coinnum] = 1;
      cannonsetup = true;
   }
}
