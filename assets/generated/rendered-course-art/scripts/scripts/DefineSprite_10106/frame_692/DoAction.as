_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i></b> If this one was just sitting there, then that means Bowser didn\'t destroy them all, but just scattered them!";
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
