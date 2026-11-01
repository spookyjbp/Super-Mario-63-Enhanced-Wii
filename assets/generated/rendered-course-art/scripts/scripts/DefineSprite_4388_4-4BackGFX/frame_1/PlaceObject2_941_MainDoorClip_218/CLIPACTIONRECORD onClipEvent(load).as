onClipEvent(load){
   doorplace = "4-5-1";
   doortype = 5;
   if(_root.Starnum == 1)
   {
      locked = true;
      keytype = 1;
      doorlockfunction = function()
      {
         _root.Course.Char.attack = true;
         _root.Course.Char.attackFrame = "Sign";
         _root.Course.Char.xspeed = 0;
         _root.PlayMessage("A Ghostly Green light glows from the other side of the door, but ithe door won\'t open.");
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
            if(_root.KeySPACE() && count > 5)
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
}
