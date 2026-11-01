_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad:</i><b>  ....Mario is Bowser\'s archenemy;  Bowser would never let Mario get that close to stopping him.";
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
