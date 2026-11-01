stop();
onEnterFrame = function()
{
   if(_root.TotalStars >= 1 && this.box.hitTest(_root.Course.Char.charClip.feet2) && _root.Course.Char.attack == false && _root.KeyUP() && !_root.KeyLEFT() && !_root.KeyRIGHT())
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.attackFrame = "Painting";
      play();
   }
};
