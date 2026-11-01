_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i>    ......You have 2 Days left to decide............";
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
