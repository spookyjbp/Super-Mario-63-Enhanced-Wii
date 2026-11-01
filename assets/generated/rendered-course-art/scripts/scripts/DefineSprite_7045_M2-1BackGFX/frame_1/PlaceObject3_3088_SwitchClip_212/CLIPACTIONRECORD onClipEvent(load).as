onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-1WaterLevel"] = -430;
      onEnterFrame = function()
      {
         if(_root["M2-1WaterLevel"] !== -430)
         {
            play();
         }
      };
   };
}
