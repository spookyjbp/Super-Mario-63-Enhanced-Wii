_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i> And now that the meteor is close enough, theres no way to stop it!";
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
