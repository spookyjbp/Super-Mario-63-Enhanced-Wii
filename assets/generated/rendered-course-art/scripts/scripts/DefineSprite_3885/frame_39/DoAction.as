stop();
_root.bgsoundFX = new Sound(this);
_root.bgsoundFX.attachSound("thwomp-2");
_root.bgsoundFX.start(0,1);
_root.bgsoundFX.setVolume(_root.BgVolume);
onEnterFrame = function()
{
   _root.StopBGsong();
   _root.playpause = true;
   onEnterFrame = function()
   {
      _root.Camcenter = this;
      _root.PauseGame = false;
      count = 0;
      _root.PlayMessage("Next...time...I\'ll...be...stronger...........");
      onEnterFrame = function()
      {
         _root.playpause = true;
         count++;
         if(_root.KeyZ() && count > 5)
         {
            _root.StopMessage();
            count = 0;
            onEnterFrame = function()
            {
               count++;
               if(count > 5)
               {
                  _root.bgsoundFX = new Sound(this);
                  _root.bgsoundFX.attachSound("Explode9");
                  _root.bgsoundFX.start(0,1);
                  _root.bgsoundFX.setVolume(_root.BgVolume);
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
            };
         }
      };
   };
};
