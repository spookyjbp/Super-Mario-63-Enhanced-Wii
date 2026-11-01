stop();
onEnterFrame = function()
{
   _root.PauseGame = false;
   if(_root.Course.Char.yspeed > -2)
   {
      play();
   }
};
