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
      if(landedon == true && _root.Course.Char.yspeed >= -3 || _root.Course.Char._y - _root.Course.BackGFX._y > -1000)
      {
         played = true;
      }
   }
   else
   {
      stop();
   }
};
