_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i></b> I have helped you save Mushroom Kingdom; however, that doesn\'t mean we aren\'t enemies...";
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
