if(_root["coindata" + _root.playingcourse][coinnum] !== 1)
{
   _root["coindata" + _root.playingcourse][coinnum] = 1;
   if(stuffinside == "1coin")
   {
      _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true});
   }
   else if(stuffinside == "3coins")
   {
      createCoin = function()
      {
         _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true});
      };
      createCoins(3,1);
   }
   else if(stuffinside == "5coins")
   {
      createCoin = function()
      {
         _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true,yspeed:Math.random() * 5 - 7,xspeed:Math.random() * 10 - 5});
      };
      createCoins(5,2);
   }
   else if(stuffinside == "10coins")
   {
      createCoin = function()
      {
         _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true,yspeed:Math.random() * 6 - 10,xspeed:Math.random() * 12 - 6});
      };
      createCoins(10,3);
   }
   else if(stuffinside == "20coins")
   {
      createCoin = function()
      {
         _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true,yspeed:Math.random() * 7 - 15,xspeed:Math.random() * 14 - 7});
      };
      createCoins(20,5);
   }
   else if(stuffinside == "30coins")
   {
      createCoin = function()
      {
         _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true,yspeed:Math.random() * 10 - 15,xspeed:Math.random() * 16 - 8});
      };
      createCoins(30,6);
   }
   else if(stuffinside == "100coins")
   {
      createCoin = function()
      {
         _root.Course.BackGFX.attachMovie("GoldCoin","GoldCoin" + _root.Course.BackGFX.getNextHighestDepth(),_root.Course.BackGFX.getNextHighestDepth(),{_x:_X - 5,_y:_Y - 12,bounce:true,yspeed:Math.random() * 15 - 18,xspeed:Math.random() * 18 - 9});
      };
      createCoins(100,10);
   }
   else if(stuffinside != "bluecoin")
   {
      if(stuffinside == "mushroom")
      {
      }
   }
}
