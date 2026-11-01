stop();
waittoopen = false;
_root.unlockdooritem = "";
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.Course.Char.air == false)
      {
         if((_root.KeyUP() || _root.KeyTapUP()) && !_root.KeyRIGHT() && !_root.KeyLEFT() && walk !== "front")
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            if(dooropenarray !== undefined)
            {
               if(_root.DoorOpenedArray[dooropenarray] == false && locked == true && removelock == true)
               {
                  _root.DoorOpenedArray[dooropenarray] = true;
                  _root.Course.Char.attackFrame = "UnlockDoor";
                  if(unlockitem !== undefined)
                  {
                     _root.heldItemObtained = unlockitem;
                     _root.unlockdooritem = "Item";
                  }
                  else
                  {
                     _root.unlockdooritem = "";
                  }
                  waittoopen = true;
               }
            }
            if(openwithkey == true)
            {
               waittoopen = true;
               _root.Course.Char.attackFrame = "UnlockDoor";
               if(unlockitem !== undefined)
               {
                  _root.heldItemObtained = unlockitem;
                  _root.unlockdooritem = "Item";
               }
               else
               {
                  _root.unlockdooritem = "";
               }
            }
            if(waittoopen == false)
            {
               if(walk == "far")
               {
                  _root.Course.Char.attackFrame = "FarDoor";
               }
               else
               {
                  _root.Course.Char.attackFrame = "Door";
               }
               _root.door = doorplace;
               gotoAndStop("play");
               play();
            }
            else
            {
               onEnterFrame = function()
               {
                  if(_root.PauseGame == false)
                  {
                     _root.Course.Char.attack = true;
                     _root.Course.Char.attackFrame = "UnlockDoor";
                     X = _X + _parent._x;
                     _root.Course.Char._x += (X - _root.Course.Char._x) / 5;
                     if(_root.Course.Char.charClip.frame.done == true)
                     {
                        if(walk == "far")
                        {
                           _root.Course.Char.attackFrame = "FarDoor";
                        }
                        else
                        {
                           _root.Course.Char.attackFrame = "Door";
                        }
                        _root.door = doorplace;
                        gotoAndStop("play");
                        play();
                        onEnterFrame = null;
                     }
                  }
               };
            }
         }
         else if(walk == "front" && (_root.KeyDOWN() || _root.KeyTapDOWN()) && !_root.KeyRIGHT() && !_root.KeyLEFT())
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.attackFrame = "FrontDoor";
            _root.door = doorplace;
            gotoAndStop("play");
            play();
         }
      }
   }
};
