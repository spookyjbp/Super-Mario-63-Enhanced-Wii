stop();
onEnterFrame = function()
{
   _root.PauseGame = false;
   if(_root.KeyZ())
   {
      play();
      _root.StopMessage();
      onEnterFrame = function()
      {
         _root.PauseGame = false;
      };
   }
};
