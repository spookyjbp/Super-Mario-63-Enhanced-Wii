onClipEvent(load){
   doorplace = "C-5-1";
   dooropenarray = 3;
   locked = true;
   if(_root.TotalStars >= 8 || _root.CheatCastleUnlock == true)
   {
      removelock = true;
   }
   keytype = 3;
   doorlockfunction = function()
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "Sign";
      _root.Course.Char.xspeed = 0;
      _root.PlayMessage("You need a total of 8 Shine Sprites to unlock this door!");
      _root.Mariosound = new Sound(this);
      _root.Mariosound.attachSound("Bowser - Laugh Muffled");
      _root.Mariosound.start(0,1);
      _root.Mariosound.setVolume(_root.MarioVolume);
      count = 0;
      onEnterFrame = function()
      {
         X = _X + _parent._x;
         _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
         _root.playpause = true;
         count++;
         if(_root.KeyZ() && count > 5)
         {
            _root.StopMessage();
            count = 0;
            onEnterFrame = function()
            {
               count++;
               if(count > 5)
               {
                  _root.playpause = false;
                  _root.Course.Char.attack = false;
                  onEnterFrame = null;
                  gotoAndStop(1);
               }
            };
         }
      };
   };
}
