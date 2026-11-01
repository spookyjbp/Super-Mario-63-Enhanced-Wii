tellTarget(_root.Course.Char)
{
   yspeed = -4;
   if(_xscale > 0)
   {
      xspeed = 5;
   }
   else
   {
      xspeed = -5;
   }
}
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
