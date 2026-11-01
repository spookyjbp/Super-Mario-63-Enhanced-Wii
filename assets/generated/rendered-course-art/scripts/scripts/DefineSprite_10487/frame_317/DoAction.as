_root.CinemaMessageFrame = 4;
_root.CinemaMessage = "<b><i>Peach:</b></i> Now that Bowser has been destroyed, we can save Mushroom Kingdom.";
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
