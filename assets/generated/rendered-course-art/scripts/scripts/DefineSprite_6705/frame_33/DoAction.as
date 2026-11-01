_parent.hp++;
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
