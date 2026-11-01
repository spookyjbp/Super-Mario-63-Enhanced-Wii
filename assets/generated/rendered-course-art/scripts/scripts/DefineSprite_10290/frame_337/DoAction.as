_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i>  ...will expand out and rule all of the Mushroom Kingdom, with me as the Supreme Ruler!!!!!";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(cam._x > -50)
   {
      cam._x -= 0.2;
   }
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null();
      play();
   }
   lastframeZ = _root.KeyZ();
};
