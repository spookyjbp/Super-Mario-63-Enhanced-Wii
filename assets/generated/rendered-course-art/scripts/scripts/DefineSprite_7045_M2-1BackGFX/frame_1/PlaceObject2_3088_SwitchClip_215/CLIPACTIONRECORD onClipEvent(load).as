onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-1WaterLevel"] = -180;
      onEnterFrame = function()
      {
         if(_root["M2-1WaterLevel"] !== -180)
         {
            play();
         }
      };
   };
}
