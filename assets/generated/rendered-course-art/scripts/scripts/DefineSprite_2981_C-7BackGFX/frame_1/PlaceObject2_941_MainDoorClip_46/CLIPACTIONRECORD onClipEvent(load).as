onClipEvent(load){
   doorplace = "C-8-1";
   doortype = 4;
   locked = true;
   keytype = 5;
   unlockitem = "GreenStar";
   if(_root.Star[50] == true || _root.CheatCastleUnlock == true)
   {
      removelock = true;
   }
   dooropenarray = 9;
   doorlockfunction = function()
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "Sign";
      _root.Course.Char.xspeed = 0;
      _root.PlayMessage("This door won\'t open with just any number of Shine Sprites....  You\'ll need a <i>special</i> Shine Sprite to get through");
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
