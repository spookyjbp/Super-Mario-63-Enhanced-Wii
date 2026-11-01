stop();
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         _root.Camcenter = this;
         _root.PauseGame = false;
         _root.playpause = true;
      };
      play();
   }
   lastframeZ = _root.KeyZ();
   _root.Camcenter = this;
   _root.PauseGame = false;
   _root.playpause = true;
};
