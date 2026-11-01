onClipEvent(load){
   Switchfunction = function()
   {
      _root.Stats.screenfilter._alpha -= 17.5;
      _parent.w2.gotoAndPlay(1);
      onEnterFrame = function()
      {
         if(_parent.w2._currentframe == 1)
         {
            play();
         }
      };
   };
}
