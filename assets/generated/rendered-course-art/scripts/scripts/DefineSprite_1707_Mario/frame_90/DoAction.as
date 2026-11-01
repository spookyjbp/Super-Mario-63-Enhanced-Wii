_root.StopBGsong();
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Mario-Fall");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
_root.Mariosound.onSoundComplete = function()
{
   onEnterFrame = null;
   _root.LifeLost();
};
count = 0;
onEnterFrame = function()
{
   _root.PauseGame = false;
   count++;
   if(count > 160)
   {
      onEnterFrame = null;
      _root.LifeLost();
   }
};
