stop();
onEnterFrame = function()
{
   if(this.hitTest((_root.Course.Char._x - _root.Course.Char.wide * 1.5) * _root.coursescale / 100 + _root.Course._x,(_root.Course.Char._y - _root.Course.Char.tall / 2) * _root.coursescale / 100 + _root.Course._y,true) && _root.KeySPACE() && _root.Power >= 0 && _root.WaterAmount > 0 && _root.Fluddpow == "T" && _root.Course.Char.xspeed < -7)
   {
      _root.Course.Platforms.turbodoor.gotoAndStop(2);
      play();
      onEnterFrame = null;
   }
};
