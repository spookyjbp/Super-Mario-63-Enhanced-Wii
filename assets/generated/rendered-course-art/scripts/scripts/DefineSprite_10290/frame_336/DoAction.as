_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i>  ...those loyal Koopa\'s of which were protected within my Castle...";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(cam._x > -50)
   {
      cam._x -= 0.3;
   }
   else if(cam._x > -80)
   {
      cam._x -= 0.3;
      cam._x *= 0.998;
   }
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null();
      play();
   }
   lastframeZ = _root.KeyZ();
};
