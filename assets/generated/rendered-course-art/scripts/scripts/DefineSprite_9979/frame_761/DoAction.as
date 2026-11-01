_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i></b> Let the Invasion... <b>BEGIN!!</b>";
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
