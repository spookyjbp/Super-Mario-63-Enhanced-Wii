onClipEvent(load){
   doorplace = "C-13-1";
   locked = true;
   keytype = 1;
   if(_root.TotalStars >= 32 && _root.CurrentPlayer == "Mario" || _root.CheatCastleUnlock == true)
   {
      removelock = true;
   }
   doorlockfunction = function()
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "Sign";
      _root.Course.Char.xspeed = 0;
      if(_root.CurrentPlayer !== "Mario" && _root.TotalStars >= 32)
      {
         _root.PlayMessage("Your are not my Rival!  Go Away and bring Mario!");
      }
      else
      {
         _root.PlayMessage("You need a total of 32 Shine Sprites to unlock this door");
      }
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
                  _root.Course.Char.attack = true;
                  _root.Course.Char.xspeed = 0;
                  _root.Course.Char.attackFrame = "Door";
                  _root.door = doorplace;
                  gotoAndStop("play");
                  play();
               }
            };
         }
      };
   };
}
