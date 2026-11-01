_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>A 3rd Toad:</i>   ....Oh My Goomba!!   WE\'RE ALL GONNA DIE!!!";
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
