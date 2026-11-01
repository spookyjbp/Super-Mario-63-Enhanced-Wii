onClipEvent(load){
   if(_root.lvl4GreenSwitch == true)
   {
      activated = true;
   }
   Switchfunction = function()
   {
      _root.lvl4GreenSwitch = true;
      _root.Mariosound = new Sound(this);
      _root.Mariosound.attachSound("Boo");
      _root.Mariosound.start(0,1);
      _root.Mariosound.setVolume(_root.MarioVolume);
      count = 0;
      onEnterFrame = function()
      {
         count++;
         if(count > 5 && _root.Course.Char.air == false)
         {
            count = 0;
            _root.PlayMessage("Ghost: GAAHH! You\'ve activated 1 of the 2 Ghostly Switches!  Now you might be able to steal our locked up Star!");
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
                        _root.playpause = false;
                        _root.Course.Char.attack = false;
                        onEnterFrame = null;
                        gotoAndStop("end");
                     }
                  };
               }
            };
         }
      };
   };
}
