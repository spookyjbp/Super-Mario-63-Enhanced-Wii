_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A Toad:</i></b>......Mario is still a half-an-hour away;  that should be enough time...";
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
