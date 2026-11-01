if(blocknum == undefined)
{
   blocknum = 1;
}
if(stuffinside == undefined)
{
   stuffinside = "3coins";
}
if(reappear == undefined)
{
   reappear = true;
}
stuff.gotoAndStop(stuffinside);
if(blocknum == 1 || blocknum == 4)
{
   stuff.gotoAndStop("none");
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
if(_root["coindata" + _root.playingcourse][coinnum] == 1 && _root["coindata" + _root.playingcourse][coinnum] !== undefined)
{
   stuff.gotoAndStop("none");
}
coincreatecount = 0;
createCoins = function(a, b)
{
   onEnterFrame = function()
   {
      if(_root.PauseGame == false)
      {
         i = 0;
         while(i < b)
         {
            createCoin();
            coincreatecount++;
            if(coincreatecount >= a)
            {
               break;
            }
            i++;
         }
         if(coincreatecount >= a)
         {
            onEnterFrame = function()
            {
               if(_root.PauseGame == false)
               {
                  play();
               }
               else
               {
                  stop();
               }
            };
         }
         play();
      }
      else
      {
         stop();
      }
   };
};
