_root.CinemaMessageFrame = 4;
_root.CinemaMessage = "<b><i>Peach:</i></b> Mario!  Are you coming?...";
lastframeZ = _root.KeyZ();
stop();
count = 0;
onEnterFrame = function()
{
   if(count++ > 96 || _root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null;
      play();
   }
   lastframeZ = _root.KeyZ();
};
