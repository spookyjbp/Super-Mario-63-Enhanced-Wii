onClipEvent(load){
   Switchfunction = function()
   {
      _root.Course.BackGFX.traingles.play();
      _root.Course.Platforms.traingles.play();
      onEnterFrame = function()
      {
         if(_root.Course.BackGFX.traingles._currentframe == 1)
         {
            play();
         }
      };
   };
}
