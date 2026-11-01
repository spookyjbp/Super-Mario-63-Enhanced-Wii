onClipEvent(load){
   Switchfunction = function()
   {
      _root.Course.BackGFX["M2-1-SPL1I"].nextFrame();
      onEnterFrame = function()
      {
         if(_root.Course.BackGFX["M2-1-SPL1I"]._currentframe == 1)
         {
            play();
         }
      };
   };
}
