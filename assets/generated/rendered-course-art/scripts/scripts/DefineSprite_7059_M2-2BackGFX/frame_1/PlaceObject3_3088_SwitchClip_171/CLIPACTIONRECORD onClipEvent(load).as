onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-1WaterLevel"] = -248;
      onEnterFrame = function()
      {
         if(_root["M2-1WaterLevel"] !== -248)
         {
            play();
         }
      };
   };
}
