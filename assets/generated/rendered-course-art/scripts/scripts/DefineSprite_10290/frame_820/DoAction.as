_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad on the Right:</i></b>  .....Bowser must\'ve locked up the Princess inside his own castle,  <b>Bowser\'s Castle!</b>";
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
