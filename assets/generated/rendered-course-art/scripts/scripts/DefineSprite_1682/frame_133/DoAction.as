onEnterFrame = function()
{
   _root.PauseGame = false;
   if(_root.Course.Char.air == false)
   {
      gotoAndStop("bowserkey1land");
      play();
   }
};
