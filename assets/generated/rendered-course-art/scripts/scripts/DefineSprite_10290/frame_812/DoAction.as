_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad on the Left:</i></b>  .....That means that we can get to the final rooms of the Castle and save the Princess!!!";
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
