_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Eddie:</i>    .....Well, even if the Meteor isn\'t a bluff, we have Mario on our side.....";
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
