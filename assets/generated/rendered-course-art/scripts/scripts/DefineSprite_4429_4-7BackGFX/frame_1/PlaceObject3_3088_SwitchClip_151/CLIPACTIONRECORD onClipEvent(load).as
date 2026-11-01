onClipEvent(load){
   if(_root.lvl4YellowSwitch == true)
   {
      activated = true;
   }
   Switchfunction = function()
   {
      _root.lvl4YellowSwitch = true;
      count = 0;
      onEnterFrame = function()
      {
         count++;
         if(count > 5 && _root.Course.Char.air == false)
         {
            count = 0;
            _root.PlayMessage("Ghost: NOO!!  The Second Ghost Switch has been activated!  Quick! Ghost Squad assemble at the East Tower Chamber!");
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
