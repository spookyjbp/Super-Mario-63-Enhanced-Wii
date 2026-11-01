onClipEvent(load){
   Switchfunction = function()
   {
      _root.Course.BackGFX.waterlevel = this._y;
      onEnterFrame = function()
      {
         if(_root.Course.BackGFX.waterlevel !== this._y)
         {
            play();
         }
      };
   };
}
