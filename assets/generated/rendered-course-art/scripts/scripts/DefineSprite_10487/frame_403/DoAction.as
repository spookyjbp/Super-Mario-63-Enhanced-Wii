_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i> Normally, Bowser would never want to destroy the entire planet!  He just wants to rule it!";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null;
      play();
   }
   lastframeZ = _root.KeyZ();
};
