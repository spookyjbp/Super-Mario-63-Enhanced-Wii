_root.Course.Char._y = _root.StartY;
_root.Course.Char._x = _root.StartX;
onEnterFrame = function()
{
   _root.Camspeed = 1;
   if(_root.PauseGame == false)
   {
      play();
   }
   else
   {
      stop();
   }
};
