_root.Mariosound = new Sound(this);
_root.Mariosound.attachSound("Tick");
_root.Mariosound.start(0,1);
_root.Mariosound.setVolume(_root.MarioVolume);
onEnterFrame = function()
{
   if(_root.playpause == false && _root.PauseGame == false)
   {
      if(_alpha <= 100 && count < durations)
      {
         _alpha = _alpha + 20;
         _alpha = Math.min(_alpha,100);
      }
      count++;
      if(count / wait == Math.round(count / wait))
      {
         if(count > 0.5 * durations)
         {
            wait = 16;
            if(count > 0.75 * durations)
            {
               wait = 8;
               _alpha = 60;
            }
            if(count > 0.9 * durations)
            {
               wait = 4;
               _alpha = 40;
            }
         }
         _root.Mariosound = new Sound(this);
         _root.Mariosound.attachSound("Tick");
         _root.Mariosound.start(0,1);
         _root.Mariosound.setVolume(_root.MarioVolume);
      }
      if(count > durations)
      {
         wait = durations;
         _alpha = _alpha - 10;
         if(_alpha <= 0)
         {
            count = 0;
            _parent.SwitchPL = false;
            prevFrame();
         }
      }
   }
};
