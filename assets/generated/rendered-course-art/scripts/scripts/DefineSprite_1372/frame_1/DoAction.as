onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _root.DiveRot = frame._rotation + 90;
      waterclip._rotation = frame._rotation;
      play();
   }
   else
   {
      stop();
   }
};
