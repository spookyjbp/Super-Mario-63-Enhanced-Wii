onEnterFrame = function()
{
   if(_root.PauseGame == false)
   {
      _parent._parent.targety = Math.max(_parent._parent.targety - 25,-200);
      _rotation = _rotation - (_rotation - rotto) / speed;
      play();
   }
   else
   {
      stop();
   }
};
