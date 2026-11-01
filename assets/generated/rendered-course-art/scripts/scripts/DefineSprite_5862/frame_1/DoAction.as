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
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("BowserStep");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
