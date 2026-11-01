stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _rotation = _rotation - (_rotation - rotto) / speed;
   }
};
