_root.CinemaMessageFrame = 1;
_root.CinemaMessage = "<b><i>Edie:</i> Mario, we must hurry and get enough Shine Sprites to undo the spell and reach the Sling Star!!";
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
