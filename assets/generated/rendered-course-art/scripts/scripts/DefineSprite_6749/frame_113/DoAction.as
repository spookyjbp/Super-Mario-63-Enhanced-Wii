_root.Course.Char.attack = false;
_root.Course.Char.attackFrame = "";
_root.playpause = false;
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
