_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toads on the Left:</i></b> No!  That\'s not it!  We\'re here to celebrate the day Mario defeated Bowser...";
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
