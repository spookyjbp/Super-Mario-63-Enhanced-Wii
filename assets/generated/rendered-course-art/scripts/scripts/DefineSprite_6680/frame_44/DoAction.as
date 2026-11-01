stop();
_root.CinemaMessageFrame = 2;
_root.CinemaMessage = "<b><i>Kamek:</i></b> I\'ve wasted enough time here. So I\'ll leave you trapped here with my goomba\'s!";
lastframeZ = _root.KeyZ();
stop();
onEnterFrame = function()
{
   if(_root.KeyZ() == true && lastframeZ !== true && _root.CinemaMessageFinished == true)
   {
      _root.CinemaMessage = "";
      onEnterFrame = function()
      {
         _root.Camcenter = _root.Course.BackGFX.Target1;
         _root.PauseGame = false;
         _root.playpause = true;
      };
      play();
   }
   lastframeZ = _root.KeyZ();
   _root.Camcenter = this;
   _root.PauseGame = false;
   _root.playpause = true;
};
