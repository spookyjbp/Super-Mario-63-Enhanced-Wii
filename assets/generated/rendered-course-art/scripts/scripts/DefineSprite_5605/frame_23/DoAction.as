loops--;
if(loops <= 0)
{
   rotto = 0;
   speed = 5;
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
}
