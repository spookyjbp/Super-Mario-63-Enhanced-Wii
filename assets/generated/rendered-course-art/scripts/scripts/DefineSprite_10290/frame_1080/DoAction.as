_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i><b>  .....Then we must find a different way to get into Bowser\'s Castle....";
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
