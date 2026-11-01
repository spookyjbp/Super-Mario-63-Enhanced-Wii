_root.Course.Char.attack = true;
_root.Course.Char.attackFrame = "none";
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
