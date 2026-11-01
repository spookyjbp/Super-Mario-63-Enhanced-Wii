stop();
_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "You can use this Warp-Pipe to return to Castle.";
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
