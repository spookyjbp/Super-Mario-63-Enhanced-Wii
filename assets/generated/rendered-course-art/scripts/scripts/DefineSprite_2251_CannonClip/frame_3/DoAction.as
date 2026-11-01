stop();
onEnterFrame = function()
{
   if(this.box.hitTest(_root.Course.Char.charClip.feet) && Math.abs(_root.Course.Char.xspeed) < 7 && cannonsetup == true)
   {
      _root.Course.Char.attack = true;
      _root.Course.Char.xspeed = 0;
      _root.Course.Char.yspeed = 0;
      _root.Course.Char.attackFrame = "none";
      play();
   }
};
