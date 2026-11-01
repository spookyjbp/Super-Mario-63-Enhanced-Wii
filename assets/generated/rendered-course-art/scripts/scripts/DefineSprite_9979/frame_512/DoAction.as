_root.CinemaMessageFrame = 4;
_root.CinemaMessage = "<b><i>Peach:</i></b> Mario, you always get here so fast... we weren\'t expecting your another 30 minutes!";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null();
      play();
   }
   lastframeZ = _root.KeyZ();
};
