_root.playpause = false;
_root.Camcenter = "none";
_root.StartBossMusic();
onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      _parent._parent._alpha -= 5;
      play();
   }
};
