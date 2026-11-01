stop();
soundeth = false;
onEnterFrame = function()
{
   if(_parent.air == false && _root.PauseGame == false)
   {
      play();
      if(soundeth == false)
      {
         _root.bgsoundFX = new Sound(this);
         _root.bgsoundFX.attachSound("BowserStep");
         _root.bgsoundFX.start(0,1);
         _root.bgsoundFX.setVolume(_root.BgVolume);
         soundeth = true;
      }
   }
   else
   {
      stop();
   }
};
