_root.CinemaMessageFrame = 4;
_root.CinemaMessage = "<b><i>Peach</i></b> The Cake for Mario is still sittting in the Fridge?  We\'ll need heat it up for when Mario gets here....";
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
