onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-1WaterLevel"] = 0;
      onEnterFrame = function()
      {
         if(_root["M2-1WaterLevel"] !== 0)
         {
            play();
         }
      };
   };
}
