rotto = 25;
speed = 15;
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _rotation = _rotation - (_rotation - rotto) / speed;
      play();
   }
   else
   {
      stop();
   }
};
