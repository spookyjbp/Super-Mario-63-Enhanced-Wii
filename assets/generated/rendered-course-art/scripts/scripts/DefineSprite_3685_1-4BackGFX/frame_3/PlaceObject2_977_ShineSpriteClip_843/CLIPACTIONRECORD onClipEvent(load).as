onClipEvent(load){
   invis = true;
   starnum = 1;
   timecount = 0;
   if(_root.Course.coincountnum == undefined)
   {
      _root.Course.coincountnum = 0;
   }
   else
   {
      _root.Course.coincountnum++;
   }
   if(_root["coindata" + _root.playingcourse][0] == undefined)
   {
      _root["coindata" + _root.playingcourse] = [""];
   }
   coinnum = _root.Course.coincountnum;
   if(_root["coindata" + _root.playingcourse][coinnum] == 1)
   {
      invis = false;
   }
}
