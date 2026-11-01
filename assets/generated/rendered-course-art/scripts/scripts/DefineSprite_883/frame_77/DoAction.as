_root.playpause = false;
_root.Camcenter = "none";
_parent._parent._alpha = 10;
onEnterFrame = function()
{
   if(_root.PauseGame == true)
   {
      stop();
   }
   else
   {
      play();
   }
};
