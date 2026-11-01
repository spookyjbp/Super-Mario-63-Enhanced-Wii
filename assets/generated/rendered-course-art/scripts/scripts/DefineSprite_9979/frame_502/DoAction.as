_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A Toad:</i></b> Well, that\'s kind of ironic, since this was supposed to be a <b>surprise party for Mario</b>...";
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
