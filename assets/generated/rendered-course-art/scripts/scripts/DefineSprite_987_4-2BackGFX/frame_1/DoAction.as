gotoAndStop(_root.Starnum + 1);
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Boo");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
if(_root.Starnum == 1 && _root.lvl4BooMessage == false)
{
   _root.lvl4BooMessage = true;
   count = 0;
   onEnterFrame = function()
   {
      count++;
      if(count > 5 && _root.Course.Char.air == false && _root.Course.Char.attack == false)
      {
         count = 0;
         _root.PlayMessage("Big Boo: Mwahaha! Welcome to my Mansion, Mario.  Come on up to my Balcony, IF YOU CAN GET THERE!!  I\'ve locked all the doors that you \'humans\' need to get up to my balcony. MWAHHAAHA!");
         onEnterFrame = function()
         {
            _root.playpause = true;
            count++;
            if(_root.KeyZ() && count > 5)
            {
               _root.StopMessage();
               count = 0;
               onEnterFrame = function()
               {
                  count++;
                  if(count > 5)
                  {
                     _root.playpause = false;
                     _root.Course.Char.attack = false;
                     onEnterFrame = null;
                     gotoAndStop(1);
                  }
               };
            }
         };
      }
   };
}
if(_root.Starnum == 3 && _root.lvl4BooMessage == false)
{
   _root.lvl4BooMessage = true;
   count = 0;
   onEnterFrame = function()
   {
      count++;
      if(count > 5 && _root.Course.Char.air == false && _root.Course.Char.attack == false)
      {
         count = 0;
         _root.PlayMessage("The Mansion is empty...\n The Shine Sprite you are looking for must be in the Basement!  You can get there from the Shack outside");
         onEnterFrame = function()
         {
            _root.playpause = true;
            count++;
            if(_root.KeyZ() && count > 5)
            {
               _root.StopMessage();
               count = 0;
               onEnterFrame = function()
               {
                  count++;
                  if(count > 5)
                  {
                     _root.playpause = false;
                     _root.Course.Char.attack = false;
                     onEnterFrame = null;
                     gotoAndStop(1);
                  }
               };
            }
         };
      }
   };
}
