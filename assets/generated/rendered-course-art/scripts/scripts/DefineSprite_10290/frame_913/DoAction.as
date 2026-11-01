_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i> ...THE Meteor of Ultamite Destruction that, in 2 days, will Crash into the Castle....";
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
