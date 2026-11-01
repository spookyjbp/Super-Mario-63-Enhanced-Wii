onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.DiveRot = frame._rotation + 90;
      water._rotation = frame._rotation;
      play();
   }
   else
   {
      stop();
   }
};
