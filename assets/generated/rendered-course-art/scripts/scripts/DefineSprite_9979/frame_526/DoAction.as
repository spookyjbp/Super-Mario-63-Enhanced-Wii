_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A Toad:</i> What\'s that noise?</b>";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   count -= 1;
   count = Math.max(count,0);
   _root.bgsong.setVolume(count);
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null();
      play();
   }
   lastframeZ = _root.KeyZ();
};
