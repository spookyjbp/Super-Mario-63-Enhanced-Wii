_root.CinemaMessageFrame = 3;
_root.CinemaMessage = "<b><i>Bowser:</i></b> The key you have is cursed!   It will warp you far away from the Castle!  Mwahahahahaha!!!";
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
