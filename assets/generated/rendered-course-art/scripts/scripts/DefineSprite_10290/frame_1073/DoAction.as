_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad to Left:</i><b>  .....The only way in is through the air;  far above the gates and towers....";
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
