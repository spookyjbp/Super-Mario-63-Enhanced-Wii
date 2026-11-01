_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Edie:</i> .....We can use the Super Sling Star that is locked in the uppermost tower!!";
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
