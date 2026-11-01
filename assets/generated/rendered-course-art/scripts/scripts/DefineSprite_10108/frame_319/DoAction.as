_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i></b> The Airship is loaded and ready for attack, Master Bowser...";
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
