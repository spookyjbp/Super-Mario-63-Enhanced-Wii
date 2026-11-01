_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i>  The Orb of Power must\'ve posessed him;  he\'s trying to destroy the entire planet!!";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         _root.Camcenter = _parent.Target1;
         _root.PauseGame = false;
      };
      play();
   }
   lastframeZ = _root.KeyZ();
   _root.Camcenter = _parent.Target1;
   _root.PauseGame = false;
};
