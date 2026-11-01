_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Mario-Squish");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
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
count = 0;
