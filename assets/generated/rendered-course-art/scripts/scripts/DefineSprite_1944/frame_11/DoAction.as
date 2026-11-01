if(go == false)
{
   _root.Course.Char.attack = false;
}
else
{
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
}
