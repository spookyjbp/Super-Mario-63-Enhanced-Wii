onEnterFrame = function()
{
   if(_xscale > 0)
   {
      _root.Camcenter = _root.Course.BackGFX.Target2;
   }
   else
   {
      _root.Camcenter = _root.Course.BackGFX.Target3;
   }
   _root.PauseGame = false;
   _root.playpause = true;
};
