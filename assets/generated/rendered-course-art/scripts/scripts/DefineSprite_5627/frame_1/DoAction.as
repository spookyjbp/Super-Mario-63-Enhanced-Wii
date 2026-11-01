onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _parent._parent.yspeed *= 0.8;
      play();
   }
   else
   {
      stop();
   }
};
_parent._parent.yspeed = 5;
_parent._parent.xspeed = 0;
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("flippers");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
