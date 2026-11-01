onClipEvent(load){
   side = "Both";
   Xpower = 40;
   Ypower = 50;
   keycount = 0;
   if(_root.lvl8keyA == true)
   {
      keycount++;
   }
   if(_root.lvl8keyB == true)
   {
      keycount++;
   }
   if(_root.lvl8keyC == true)
   {
      keycount++;
   }
   if(_root.lvl8keyD == true)
   {
      keycount++;
   }
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
      cannonsetup = true;
   }
   else
   {
      cannonsetup = false;
   }
   count = 0;
}
