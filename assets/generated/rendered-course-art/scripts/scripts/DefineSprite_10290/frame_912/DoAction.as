_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i> ....Now that I have the Ancient Orb of Power in my Control, I have the ability to Summon a Meteor....";
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
