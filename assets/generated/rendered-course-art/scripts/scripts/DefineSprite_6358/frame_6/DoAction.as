stop();
onEnterFrame = function()
{
   _root.PauseGame = false;
   _root.playpause = true;
   if(!_root.KeyZ())
   {
      SPACE = false;
   }
   if(_root.KeyZ() && SPACE == false)
   {
      _root.StopMessage();
      play();
   }
};
