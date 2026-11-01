onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-3WaterLevel"] = this._y + 20;
      onEnterFrame = function()
      {
         if(_root["M2-3WaterLevel"] !== this._y + 20)
         {
            play();
         }
      };
   };
}
