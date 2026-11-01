count = 0;
wait = 32;
duration = 800;
if(sound == true)
{
   _root.Mariosound = new Sound(this);
   _root.Mariosound.attachSound("Tick");
   _root.Mariosound.start(0,1);
   _root.Mariosound.setVolume(_root.MarioVolume);
}
onEnterFrame = function()
{
   count++;
   if(count / wait == Math.round(count / wait) && count <= duration && sound == true)
   {
      if(count > 0.5 * duration)
      {
         wait = 16;
         if(count > 0.75 * duration)
         {
            wait = 8;
            play();
         }
         if(count > 0.9 * duration)
         {
            wait = 4;
            play();
         }
      }
      _root.Mariosound = new Sound(this);
      _root.Mariosound.attachSound("Tick");
      _root.Mariosound.start(0,1);
      _root.Mariosound.setVolume(_root.MarioVolume);
   }
   else if(count > duration)
   {
      wait = duration;
      if(_currentframe >= 10)
      {
         gotoAndStop(9);
      }
      prevFrame();
      if(_currentframe <= 1)
      {
         count = 0;
         stop();
         onEnterFrame = null;
      }
   }
};
