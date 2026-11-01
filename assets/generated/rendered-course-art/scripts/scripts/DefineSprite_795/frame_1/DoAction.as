stop();
_root.PauseGame = false;
rewind = false;
presskey = false;
onEnterFrame = function()
{
   if(presskey == false)
   {
      if(_root.KeyHome() && _root.PauseGame !== true)
      {
         _root.PauseGame = true;
         rewind = false;
         play();
         presskey = true;
      }
      else if(_root.KeyHome() && _root.PauseGame !== false)
      {
         rewind = true;
         presskey = true;
      }
   }
   if(!_root.KeyHome())
   {
      presskey = false;
   }
   if(_currentframe > 1 && _root.PauseGame !== true)
   {
      rewind = true;
   }
   if(rewind == true)
   {
      prevFrame();
   }
};
