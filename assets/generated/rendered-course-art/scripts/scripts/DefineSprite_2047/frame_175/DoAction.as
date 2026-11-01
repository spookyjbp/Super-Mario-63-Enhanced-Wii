_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i></b> Mwahaha! Mario, you thought I was foolish enough to give you the Key to the Basement that easily?";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         _root.PauseGame = false;
      };
      play();
   }
   lastframeZ = _root.KeyZ();
   _root.PauseGame = false;
};
