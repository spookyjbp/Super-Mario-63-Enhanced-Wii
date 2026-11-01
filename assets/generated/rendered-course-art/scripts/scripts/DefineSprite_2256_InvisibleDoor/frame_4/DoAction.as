stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.Course.Char.air == false)
      {
         if((_root.KeyUP() || _root.KeyTapUP()) && !_root.KeyRIGHT() && !_root.KeyLEFT() && walk !== "front" && walk !== "warppipe")
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            if(walk == "far")
            {
               _root.Course.Char.attackFrame = "FarDoor";
            }
            else
            {
               _root.Course.Char.attackFrame = "Door";
            }
            _root.door = doorplace;
            play();
         }
         else if(walk == "front" && (_root.KeyDOWN() || _root.KeyTapDOWN()) && !_root.KeyRIGHT() && !_root.KeyLEFT() && walk !== "warppipe")
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.attackFrame = "FrontDoor";
            _root.door = doorplace;
            play();
         }
         else if(walk == "warppipe" && (_root.KeyDOWN() || _root.KeyTapDOWN()) && !_root.KeyRIGHT() && !_root.KeyLEFT())
         {
            _root.Course.Char.attack = true;
            _root.Course.Char.xspeed = 0;
            _root.Course.Char.attackFrame = "WarpPipe";
            _root.door = doorplace;
            play();
         }
      }
   }
};
