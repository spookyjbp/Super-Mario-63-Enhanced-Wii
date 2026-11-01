_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toad on the Right:</i></b>  ....But Bowser isn\'t foolish enough to just leave the Princess inside her own Castle!";
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
