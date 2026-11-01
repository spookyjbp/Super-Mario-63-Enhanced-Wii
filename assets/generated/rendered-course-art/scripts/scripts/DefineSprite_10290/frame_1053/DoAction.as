_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad:</i><b>  ....but if Mario shows up, they would obviously refuse to let him in...";
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
