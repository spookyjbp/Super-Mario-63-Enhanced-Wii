stop();
onEnterFrame = function()
{
   _root.playpause = true;
   if(_root.KeySPACE())
   {
      play();
      _root.StopMessage();
      onEnterFrame = null;
   }
};
