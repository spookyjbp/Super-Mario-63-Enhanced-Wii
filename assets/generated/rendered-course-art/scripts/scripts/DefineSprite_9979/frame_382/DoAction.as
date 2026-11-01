_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Toads:</i></b> Woah!  Mario you surprised me!...";
lastframeZ = _root.KeyZ();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         play();
      };
   }
   lastframeZ = _root.KeyZ();
};
