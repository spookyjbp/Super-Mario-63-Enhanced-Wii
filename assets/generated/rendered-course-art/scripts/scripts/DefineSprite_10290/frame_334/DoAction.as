_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i>  Then, after my Meteor purged it of all the miserable creatures it now has, my Master Race of Koopa\'s...";
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
