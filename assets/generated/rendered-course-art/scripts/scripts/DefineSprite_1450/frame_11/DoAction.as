stop();
count;
count = 0;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      count++;
      if(count == 32)
      {
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("Mario - Snore1");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
      }
      if(count >= 54.4)
      {
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("Mario - Snore2");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
         count = 0;
      }
   }
   stop();
   if(_root.PauseGame == false)
   {
      if(_root.KeyUP() || _root.KeyRIGHT() || _root.KeyLEFT() || _root.KeyDOWN() || _root.KeySPACE())
      {
         play();
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
      }
   }
};
