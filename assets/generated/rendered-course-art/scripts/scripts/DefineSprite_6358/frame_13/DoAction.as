played = false;
landedon = false;
stop();
onEnterFrame = function()
{
   if(_root.PauseGame == false && _root.playpause == false)
   {
      if(played == true)
      {
         play();
      }
      if(landedon == true && _root.Course.Char.yspeed >= -3)
      {
         played = true;
      }
   }
   else
   {
      stop();
   }
};
