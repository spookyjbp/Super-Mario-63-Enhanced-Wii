_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>???:</i></b> I\'ve been trying to wake you up for 4 hours now!";
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
