_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>New Toad:</i></b>  ......It looks as if he just escaped from Bowser\'s clutches with the next Key.";
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
