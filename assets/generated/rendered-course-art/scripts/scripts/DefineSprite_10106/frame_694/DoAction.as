_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i></b> That means that there is probably some hope for saving the Castle and rescuing Princess Peach!";
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
