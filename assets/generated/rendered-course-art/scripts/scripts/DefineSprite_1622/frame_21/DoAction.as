stop();
onEnterFrame = function()
{
   _root.playpause = true;
   if(_root.KeyZ())
   {
      play();
      _root.StopMessage();
      onEnterFrame = null;
   }
};
