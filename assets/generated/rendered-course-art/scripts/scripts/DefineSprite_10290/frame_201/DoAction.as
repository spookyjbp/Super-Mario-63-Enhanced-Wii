_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i>  ... All the worthless creatures of Mushroom Kingdom will perish amidst an engulfing pyrotechnic Inferno!!!";
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
