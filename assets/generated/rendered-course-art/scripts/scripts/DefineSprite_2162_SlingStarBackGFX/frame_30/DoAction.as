onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      star.base._rotation += 1;
      star2.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star.base._rotation = star.base._rotation;
      _root.Course.FrontGFX[otherstar].star2.base._rotation = star.base._rotation;
      play();
   }
   else
   {
      stop();
   }
};
