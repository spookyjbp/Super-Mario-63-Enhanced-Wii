stop();
_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i></b> Ha that robot was just a diversion!!";
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
