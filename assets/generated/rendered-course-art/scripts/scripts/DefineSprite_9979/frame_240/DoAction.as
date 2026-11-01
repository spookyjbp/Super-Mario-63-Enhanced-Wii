_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A Toad:</i></b> I just finished setting up the banners inside the Castle...";
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
