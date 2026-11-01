_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>The Same Toad:</i>   .......Meter of Ultamite Destruction?!?!?!?!?!?";
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
