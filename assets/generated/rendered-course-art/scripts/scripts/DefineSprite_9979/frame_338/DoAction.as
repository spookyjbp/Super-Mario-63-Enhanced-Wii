_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A 3rd Toad:</i></b> Then it\'s the very first time Mario squashed Bowser\'s Plan, not one of the many insignificant times after...";
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
