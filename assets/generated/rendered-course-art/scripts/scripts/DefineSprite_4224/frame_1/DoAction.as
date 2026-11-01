stop();
played = false;
landedon = false;
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(landedon == true)
      {
         played = true;
      }
      if(played == true)
      {
         play();
      }
   }
   else
   {
      stop();
   }
};
