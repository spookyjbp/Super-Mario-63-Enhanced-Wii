stop();
_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "A Sling-Star has appeared.";
onEnterFrame = function()
{
   OEF();
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      play();
      onEnterFrame = function()
      {
         OEF();
      };
   }
   lastframeZ = _root.KeyZ();
};
