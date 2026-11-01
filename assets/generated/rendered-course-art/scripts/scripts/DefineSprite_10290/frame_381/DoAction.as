_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i>  You will suffer the molten madness about to be summoned upon this powerless kingdom!!";
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
