stop();
_parent.yspeed = -8;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
