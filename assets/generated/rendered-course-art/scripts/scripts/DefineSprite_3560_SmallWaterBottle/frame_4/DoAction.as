_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("Fludd Nozzle");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
