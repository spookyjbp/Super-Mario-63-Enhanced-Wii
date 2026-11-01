_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("VolcanoErupting");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
onEnterFrame = function()
{
   _root.Camcenter = _parent._parent;
   if(_root.PauseGame == false)
   {
      if(_root.Course.Char.air == false)
      {
         _root.Course.Char.attack = false;
         _root.playpause = true;
      }
      _parent._parent.yspeed *= 0.7;
      play();
   }
   else
   {
      stop();
   }
};
