_root.CinemaMessageFrame = 3;
_root.CinemaMessage = " <b><i>Bowser:</i>  ....to crash down into Princess Peach\'s Castle incinerating the entire pitiful landscape into a raging Inferno...";
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
