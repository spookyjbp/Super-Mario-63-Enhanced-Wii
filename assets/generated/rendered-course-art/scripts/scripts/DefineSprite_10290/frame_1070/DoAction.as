_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad to Left:</i><b>  ....No... Just last month, me and my family went to on a vacation to Bowser\'s Castle...";
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
