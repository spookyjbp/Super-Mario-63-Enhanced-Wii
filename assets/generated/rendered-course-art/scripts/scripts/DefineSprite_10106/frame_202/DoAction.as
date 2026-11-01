_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i></b> Also, the Shine Sprite Orb must\'ve been broken, since Mushroom Kingdom is now shrouded in darkness!";
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
