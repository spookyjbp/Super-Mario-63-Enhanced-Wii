_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Another Toad:</i></b> Hey!  How about we have that Party now?";
lastframeZ = _root.KeyZ();
stop();
count = 0;
onEnterFrame = function()
{
   if(count++ > 96 || _root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null;
      play();
   }
   lastframeZ = _root.KeyZ();
};
