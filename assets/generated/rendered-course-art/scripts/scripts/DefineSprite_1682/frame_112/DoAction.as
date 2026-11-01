stop();
onEnterFrame = function()
{
   _root.PauseGame = false;
   if(_root.Course.Char.air == false)
   {
      play();
      onEnterFrame = function()
      {
         _root.PauseGame = false;
      };
   }
};
