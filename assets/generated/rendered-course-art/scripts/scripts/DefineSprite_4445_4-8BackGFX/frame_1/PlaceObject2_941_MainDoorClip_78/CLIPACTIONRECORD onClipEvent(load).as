onClipEvent(load){
   locked = true;
   doorlockfunction = function()
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.attackFrame = "Sign";
      _root.Course.Char.xspeed = 0;
      _root.PlayMessage("It seems as if this door leads outside, but unfortunatly, it\'s locked");
      _root.Mariosound = new Sound(this);
      _root.Mariosound.attachSound("Boo");
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
   doortype = 5;
}
