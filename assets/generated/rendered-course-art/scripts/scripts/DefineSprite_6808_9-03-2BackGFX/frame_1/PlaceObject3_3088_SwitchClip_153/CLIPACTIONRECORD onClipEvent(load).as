onClipEvent(load){
   gotoAndStop("end");
   onEnterFrame = function()
   {
      if(_root.Course.BackGFX.waterlevel !== -60)
      {
         play();
      }
   };
   Switchfunction = function()
   {
      _root.Course.BackGFX.waterlevel = -60;
      onEnterFrame = function()
      {
         if(_root.Course.BackGFX.waterlevel !== -60)
         {
            play();
         }
      };
   };
}
