stop();
onEnterFrame = function()
{
   if(this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.Course.Char.air == false && _root.KeyUP() && !_root.KeyLEFT() && !_root.KeyRIGHT())
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.attackFrame = "Painting";
      onEnterFrame = function()
      {
         _root.Camcenter = this;
      };
      play();
   }
};
