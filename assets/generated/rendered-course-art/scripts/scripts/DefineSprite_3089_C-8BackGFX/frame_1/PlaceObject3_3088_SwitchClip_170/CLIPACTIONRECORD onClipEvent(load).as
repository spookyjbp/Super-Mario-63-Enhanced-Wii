onClipEvent(load){
   gotoAndStop("end");
   onEnterFrame = function()
   {
      if(_root.C8Switch == true)
      {
         play();
      }
   };
   Switchfunction = function()
   {
      count = 0;
      onEnterFrame = function()
      {
         count++;
         if(count > 5)
         {
            count = 0;
            _root.PlayMessage("Oh No! The Castle\'s Basement Drain has been opened! All the water will now be drained out!");
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
                        _root.C8Switch = false;
                        _root.playpause = false;
                        _root.Course.Char.attack = false;
                        onEnterFrame = function()
                        {
                           if(_root.C8Switch == true)
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
