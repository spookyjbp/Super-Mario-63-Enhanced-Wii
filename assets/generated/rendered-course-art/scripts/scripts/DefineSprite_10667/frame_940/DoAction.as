_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A Toad:</i> Congratulations, Mario, I didn\'t think you could\'ve done it, but you did.";
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
