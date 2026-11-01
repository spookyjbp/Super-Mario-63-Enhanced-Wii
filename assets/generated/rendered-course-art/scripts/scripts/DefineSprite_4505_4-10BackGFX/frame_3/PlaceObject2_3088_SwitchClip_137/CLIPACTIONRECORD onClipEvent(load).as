onClipEvent(load){
   Switchfunction = function()
   {
      _root.Stats.screenfilter._alpha -= 17.5;
      _parent.w1.gotoAndPlay(1);
      onEnterFrame = function()
      {
         if(_parent.w1._currentframe == 1)
         {
            play();
         }
      };
   };
}
