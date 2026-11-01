stop();
_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i></b> It\'s too late Mario;  the Meteor is fully charged and ready to launch!!!   Soon, it will crash into Peach\'s Castle and decimate the World!!!  You Can Not Do Anything To Stop Me Now!!!!!";
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
