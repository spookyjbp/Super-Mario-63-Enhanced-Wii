_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad to Right:</i>    ......But didn\'t you hear Bowser?!?!   He has this <i>Orb of Power</i> or something!!";
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
