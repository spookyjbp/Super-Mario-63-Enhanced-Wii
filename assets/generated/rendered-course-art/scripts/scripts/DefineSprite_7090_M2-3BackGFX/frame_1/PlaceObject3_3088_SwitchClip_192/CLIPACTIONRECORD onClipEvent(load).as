onClipEvent(load){
   Switchfunction = function()
   {
      _root["M2-3WaterLevel"] = this._y;
      onEnterFrame = function()
      {
         if(_root["M2-3WaterLevel"] !== this._y)
         {
            play();
         }
      };
   };
}
