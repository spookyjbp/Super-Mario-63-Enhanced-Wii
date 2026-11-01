_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i> *gasp*  That\'s it!!  We will reverse Bowser\'s force field!";
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
