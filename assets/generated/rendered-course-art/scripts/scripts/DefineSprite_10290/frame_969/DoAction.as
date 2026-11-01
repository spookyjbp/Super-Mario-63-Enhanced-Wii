_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i>    ......I\'m sure that Bowser is just making all that stuff up....";
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
