onClipEvent(load){
   Switchfunction = function()
   {
      count = 0;
      onEnterFrame = function()
      {
         count++;
         if(count > 5)
         {
            count = 0;
            _root.PlayMessage("Oh No! The Castle\'s Lake Drain has been opened!  Now Water is going to be flooding in! Swim! Swim! Before you drown!");
            onEnterFrame = function()
            {
               _root.playpause = true;
               count++;
               if((_root.KeySPACE() || _root.KeyZ() || _root.KeyX()) && count > 5)
               {
                  _root.StopMessage();
                  count = 0;
                  onEnterFrame = function()
                  {
                     count++;
                     if(count > 5)
                     {
                        _root.C8Switch = true;
                        _root.playpause = false;
                        _root.Course.Char.attack = false;
                        onEnterFrame = function()
                        {
                           if(_root.C8Switch == false)
                           {
                              play();
                           }
                        };
                        gotoAndStop("end");
                     }
                  };
               }
            };
         }
      };
   };
}
