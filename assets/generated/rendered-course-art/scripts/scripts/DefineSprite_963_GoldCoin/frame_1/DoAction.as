if(bounce == undefined)
{
   bounce = false;
   hitcoin = true;
}
else
{
   hitcoin = false;
   bouncefunction();
   gotoAndStop(3);
}
if(bounce == false)
{
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
   if(_root["coindata" + _root.playingcourse][coinnum] == 1 && _root["coindata" + _root.playingcourse][coinnum] !== undefined)
   {
      gotoAndStop("end");
   }
}
else
{
   appearcount = 0;
   if(yspeed == undefined)
   {
      yspeed = -7;
   }
   if(xspeed == undefined)
   {
      xspeed = Math.random() * 4 - 2;
   }
}
