onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-1WaterLevel"] = -80;
      onEnterFrame = function()
      {
         if(_root["M2-1WaterLevel"] !== -80)
         {
            play();
         }
      };
   };
}
