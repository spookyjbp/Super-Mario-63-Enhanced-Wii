stop();
onEnterFrame = function()
{
   if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.Course.Char.air == false)
   {
      if(_root.KeyUP() && !_root.KeyRIGHT() && !_root.KeyLEFT())
      {
         _root.Course.Char.attack = true;
         _root.Course.Char.xspeed = 0;
         _root.Course.Char.attackFrame = "BigDoor";
         _root.door = doorplace;
         play();
      }
   }
};
