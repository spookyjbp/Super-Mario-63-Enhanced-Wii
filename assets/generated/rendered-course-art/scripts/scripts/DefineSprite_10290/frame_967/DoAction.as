_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>2nd Toad:</i>   .........NO!!! Everyone!! WE have to run around in Circles!!!!";
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
