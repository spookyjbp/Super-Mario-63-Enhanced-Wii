_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad:</i><b>  ....The guards at the gate around Bowser\'s Castle would let in a Toad or Goomba...";
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
