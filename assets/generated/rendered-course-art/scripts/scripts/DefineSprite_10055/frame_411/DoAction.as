_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i></b> The Shine Sprites will drift around aimlessly. Eventually they will be lost forever...";
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
