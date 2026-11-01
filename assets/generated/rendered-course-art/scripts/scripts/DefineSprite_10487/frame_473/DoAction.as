_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i>  There.  We need to get out of here fast! The whole place will start to explode!";
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
