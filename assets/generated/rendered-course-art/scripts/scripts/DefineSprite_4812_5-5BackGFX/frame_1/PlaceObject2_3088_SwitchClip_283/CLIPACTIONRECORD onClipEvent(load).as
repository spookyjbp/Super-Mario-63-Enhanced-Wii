onClipEvent(load){
   Switchfunction = function()
   {
      _parent.SwitchPL = true;
      onEnterFrame = function()
      {
         if(_parent.SwitchPL !== true)
         {
            play();
         }
      };
   };
}
