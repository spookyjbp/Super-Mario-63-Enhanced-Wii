onClipEvent(load){
   _alpha = 0;
   DirectionX = "Left";
   DirectionY = "Up";
   distanceX = 0;
   distanceY = 300;
   accelX = 0;
   accelY = 0.2;
   speedX = 0;
   speedY = 3;
   scale = 3;
   disapearcount = 110;
   count = 0;
   OEF = function()
   {
      count++;
      _root.Camcenter = this;
      if(count > 5)
      {
         count = 0;
         _root.PlayMessage("A platform has appeareed.");
         OEF = function()
         {
            _root.playpause = true;
            _root.Camcenter = this;
            count++;
            if((_root.KeySPACE() || _root.KeyZ() || _root.KeyX()) && count > 5)
            {
               _root.StopMessage();
               _root.Camcenter = "None";
               count = 0;
               OEF = function()
               {
                  count++;
                  if(count > 5)
                  {
                     _root.playpause = false;
                     OEF = null;
                  }
               };
            }
         };
      }
   };
}
