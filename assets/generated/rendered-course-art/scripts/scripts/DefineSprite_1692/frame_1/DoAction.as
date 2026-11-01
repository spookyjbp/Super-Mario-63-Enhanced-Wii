onEnterFrame = function()
{
   _root.PauseGame = false;
};
_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("mario-drown");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
