_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Another Toad:</i></b> But Bowser has been defeated many times already, not just once...";
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
