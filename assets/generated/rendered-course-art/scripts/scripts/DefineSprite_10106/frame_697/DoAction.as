_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i></b> Come on Mario, lets go put the Shine Sprite in the Castle!";
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
