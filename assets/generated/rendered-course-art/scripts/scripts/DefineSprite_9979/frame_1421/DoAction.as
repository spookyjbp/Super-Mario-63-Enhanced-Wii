_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i></b> Your Castle mine, Princess!";
lastframeZ = _root.KeyZ();
stop();
randomnum = 0;
onEnterFrame = function()
{
   if(randomnum += random(10) + 3 > 160)
   {
      randomnum = 0;
      _root.QuakeX = random(10) - 5;
      _root.QuakeY = random(10) - 5;
   }
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = null();
      play();
   }
   lastframeZ = _root.KeyZ();
};
