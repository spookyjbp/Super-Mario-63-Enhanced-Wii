onClipEvent(load){
   invis = true;
   starnum = 62;
   timecount = 0;
   if(_parent.coincountnum == undefined)
   {
      _parent.coincountnum = 0;
   }
   else
   {
      _parent.coincountnum++;
   }
   if(_root["coindata" + _root.playingcourse][0] == undefined)
   {
      _root["coindata" + _root.playingcourse] = [""];
   }
   coinnum = _parent.coincountnum;
   if(_root["coindata" + _root.playingcourse][coinnum] == 1)
   {
      invis = false;
   }
}
